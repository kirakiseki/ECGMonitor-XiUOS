/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiUOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
 * @file sensor.c
 * @brief Implement the sensor framework management, registration and done
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2021.03.24
 */

#include <sensor.h>

/* Sensor quantity list table */
static DoublelistType quant_table[SENSOR_QUANTITY_END];

/* Sensor device list */
static DoublelistType sensor_device_list;

/* Sensor quantity list lock */
static pthread_mutex_t quant_table_lock;

/* Sensor device list lock */
static pthread_mutex_t sensor_device_list_lock;

/**
 * @description: Init sensor framework
 * @return 0
 */
int SensorFrameworkInit(void)
{
    int ret = 0;
    for (int i = 0; i < SENSOR_QUANTITY_END; i++)
        AppInitDoubleList(&quant_table[i]);
    AppInitDoubleList(&sensor_device_list);

    ret = PrivMutexCreate(&quant_table_lock, 0);
    if(ret < 0) {
        printf("quant_table_lock mutex create failed.\n");
    }
    ret = PrivMutexCreate(&sensor_device_list_lock, 0);
    if(ret < 0) {
        printf("sensor_device_list_lock mutex create failed.\n");
    }

    return 0;
}

/* =============================  Sensor device interface operations ============================= */

/**
 * @description: Find sensor device by name
 * @param name - name string
 * @return sensor device pointer
 */
static struct SensorDevice *SensorDeviceFindByName(const char *name)
{
    struct SensorDevice *ret = NULL;
    struct DoublelistNode *node;

    if (name == NULL)
        return NULL;

    PrivMutexObtain(&sensor_device_list_lock);
    DOUBLE_LIST_FOR_EACH(node, &sensor_device_list) {
        struct SensorDevice *sdev =CONTAINER_OF(node,
                struct SensorDevice, link);
        if (strncmp(sdev->name, name, NAME_NUM_MAX) == 0) {
            ret = sdev;
            break;
        }
    }
    PrivMutexAbandon(&sensor_device_list_lock);

    return ret;
}

/**
 * @description: Check whether the sensor is capable
 * @param sdev - sensor device pointer
 * @param ability - the ability to detect certain data
 * @return success: true , failure: false
 */
inline int SensorDeviceCheckAbility(struct SensorDevice *sdev, uint32_t ability)
{
    return (sdev->info->ability & ability) != 0;
}

/**
 * @description: Register the sensor to the linked list
 * @param sdev - sensor device pointer
 * @return success: 0 , failure: -1
 */
int SensorDeviceRegister(struct SensorDevice *sdev)
{
    if (sdev == NULL)
        return -1;

    if (SensorDeviceFindByName(sdev->name) != NULL) {
        printf("%s: sensor with the same name already registered\n", __func__);
        return -1;
    }

    sdev->ref_cnt = 0;
    AppInitDoubleList(&sdev->quant_list);

    PrivMutexObtain(&sensor_device_list_lock);
    AppDoubleListInsertNodeAfter(&sensor_device_list, &sdev->link);
    PrivMutexAbandon(&sensor_device_list_lock);

    return 0;
}

/**
 * @description: Unregister the sensor from the linked list
 * @param sdev - sensor device pointer
 * @return 0
 */
int SensorDeviceUnregister(struct SensorDevice *sdev)
{
    if (!sdev)
        return -1;
    PrivMutexObtain(&sensor_device_list_lock);
    AppDoubleListRmNode(&sdev->link);
    PrivMutexAbandon(&sensor_device_list_lock);

    return 0;
}

/**
 * @description: Open sensor device
 * @param sdev - sensor device pointer
 * @return success: 0 , failure: other
 */
static int SensorDeviceOpen(struct SensorDevice *sdev)
{
    if (!sdev)
        return -1;

    int result = 0;

    if (sdev->done->open != NULL)
        result = sdev->done->open(sdev);

    if (result >= 0) {
        printf("Device %s open success.\n", sdev->name);
    }else{
        printf("Device %s open failed(%d).\n", sdev->name, result);
        memset(sdev, 0, sizeof(struct SensorDevice));
    }
    
    return result;
}

/**
 * @description: Close sensor device
 * @param sdev - sensor device pointer
 * @return success: 0 , failure: other
 */
static int SensorDeviceClose(struct SensorDevice *sdev)
{
    int result = 0;

    if (sdev->fd >= 0)
        PrivClose(sdev->fd);

    if (sdev->done->close != NULL)
        result = sdev->done->close(sdev);

    if (result >= 0)
        printf("%s successfully closed.\n", sdev->name);
    else
        printf("Closed %s failure.\n", sdev->name);

    return result;
}

/* =============================  Sensor quantity interface operations ============================= */

/**
 * @description: Find sensor quantity by name
 * @param name - name string
 * @param type - the quantity required
 * @return sensor quantity pointer
 */
struct SensorQuantity *SensorQuantityFind(const char *name,
        enum SensorQuantityType type)
{
    struct SensorQuantity *ret = NULL;
    struct DoublelistNode *node;

    if (name == NULL || type < 0 || type >= SENSOR_QUANTITY_END)
        return NULL;

    PrivMutexObtain(&quant_table_lock);
    DOUBLE_LIST_FOR_EACH(node, &quant_table[type]) {
        struct SensorQuantity *quant =CONTAINER_OF(node,
                struct SensorQuantity, link);
        if (strncmp(quant->name, name, NAME_NUM_MAX) == 0) {
            ret = quant;
            break;
        }
    }
    PrivMutexAbandon(&quant_table_lock);

    return ret;
}

/**
 * @description: Register the quantity to the linked list
 * @param quant - sensor quantity pointer
 * @return success: 0 , failure: -1
 */
int SensorQuantityRegister(struct SensorQuantity *quant)
{
    if (quant == NULL)
        return -1;

    if (SensorDeviceFindByName(quant->sdev->name) == NULL) {
        if(SensorDeviceRegister(quant->sdev) != 0)
            return -1;
    }

    PrivMutexObtain(&quant_table_lock);
    AppDoubleListInsertNodeAfter(&quant->sdev->quant_list, &quant->quant_link);
    AppDoubleListInsertNodeAfter(&quant_table[quant->type], &quant->link);
    PrivMutexAbandon(&quant_table_lock);

    return 0;
}

/**
 * @description: Unregister the quantity from the linked list
 * @param quant - sensor quantity pointer
 * @return 0
 */
int SensorQuantityUnregister(struct SensorQuantity *quant)
{
    if (!quant)
        return -1;
    PrivMutexObtain(&quant_table_lock);
    AppDoubleListRmNode(&quant->quant_link);
    AppDoubleListRmNode(&quant->link);
    PrivMutexAbandon(&quant_table_lock);

    return 0;
}

/**
 * @description: Open the sensor quantity
 * @param quant - sensor quantity pointer
 * @return success: 0 , failure: other
 */
int SensorQuantityOpen(struct SensorQuantity *quant)
{
    if (!quant)
        return -1;

    int ret = 0;
    struct SensorDevice *sdev = quant->sdev;

    if (!sdev)
        return -1;

    if (sdev->ref_cnt == 0) {
        ret = SensorDeviceOpen(sdev);
        if (ret < 0) {
            printf("%s: open sensor device failed\n", __func__);
            return ret;
        }
    }
    sdev->ref_cnt++;

    return ret;
}

/**
 * @description: Close sensor quantity
 * @param quant - sensor quantity pointer
 * @return success: 0 , failure: other
 */
int SensorQuantityClose(struct SensorQuantity *quant)
{
    if (!quant)
        return -1;

    int ret = 0;
    struct SensorDevice *sdev = quant->sdev;

    if (!sdev)
        return -1;

    if (sdev->ref_cnt == 0)
        return ret;

    sdev->ref_cnt--;
    if (sdev->ref_cnt == 0)
        ret = SensorDeviceClose(sdev);

    return ret;
}

/**
 * @description: Read quantity current value
 * @param quant - sensor quantity pointer
 * @return quantity value
 */
int SensorQuantityReadValue(struct SensorQuantity *quant)
{
    if (!quant)
        return -1;

    int result = 0;
    struct SensorDevice *sdev = quant->sdev;

    if (!sdev)
        return -1;

    if (quant->ReadValue != NULL) {
        result = quant->ReadValue(quant);
    }

    return result;
}

/**
 * @description: Read quantity decimal point
 * @param quant - sensor quantity pointer
 * @return decimal point num, 0: 0 1: 0.1 2: 0.01 3: 0.001
 */
int SensorQuantityReadDecimalPoint(struct SensorQuantity *quant)
{
    if (!quant)
        return -1;

    int decimal_point = 0;
    struct SensorDevice *sdev = quant->sdev;

    if (!sdev)
        return -1;

    if (quant->ReadValue != NULL) {
        decimal_point = quant->ReadDecimalPoint(quant);
    }

    return decimal_point;
}

/**
 * @description: Configure quantity mode
 * @param quant - sensor quantity pointer
 * @param cmd - mode command
 * @return success: 0 , failure: other
 */
int SensorQuantityControl(struct SensorQuantity *quant, int cmd)
{
    if (!quant)
        return -1;

    if (quant->sdev->done->ioctl != NULL) {
        return quant->sdev->done->ioctl(quant->sdev, cmd);
    }

    return -1;
}


/* =============================  Check function ============================= */

/**
 * @description: CRC16 check
 * @param data sensor receive buffer
 * @param length sensor receive buffer minus check code
 * @return check code
 */
uint32_t Crc16(uint8_t * data, uint8_t length)
{
    int j;
    unsigned int reg_crc=0xFFFF;
    
    while (length--) {
        reg_crc ^= *data++;
        for (j=0;j<8;j++) {
            if(reg_crc & 0x01)
                reg_crc=reg_crc >>1 ^ 0xA001;
            else
                reg_crc=reg_crc >>1;
        }
    }
    
    return reg_crc;
}

/**
 * @description: The checksum
 * @param data sensor receive buffer
 * @param head not check head length
 * @param length sensor receive buffer minus check code
 * @return check code
 */
uint8_t GetCheckSum(uint8_t *data, uint8_t head, uint8_t length)
{
    uint8_t i;
    uint8_t checksum = 0;
    for (i = head; i < length; i++) {
        checksum += data[i];
    }
    checksum = ~checksum + 1;
    
    return checksum;
}
