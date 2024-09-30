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
 * @file qs-fx.c
 * @brief qs-fx wind direction driver base sensor
 * @version 1.1
 * @author AIIT XUOS Lab
 * @date 2021.12.10
 */

#include <sensor.h>

static struct SensorDevice qs_fx;
static char instructions[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x39};

static struct SensorProductInfo info =
{
    SENSOR_ABILITY_WINDDIRECTION,
    "清易电子",
    "QS-FX",
};

/**
 * @description: Open QS-FX voice device
 * @param sdev - sensor device pointer
 * @return success: 1 , failure: other
 */
static int SensorDeviceOpen(struct SensorDevice *sdev)
{
    int result = 0;

    sdev->fd = PrivOpen(SENSOR_DEVICE_QS_FX_DEV, O_RDWR);
    if (sdev->fd < 0) {
        printf("open %s error\n", SENSOR_DEVICE_QS_FX_DEV);
        return -1;
    }
    
    struct SerialDataCfg cfg;
    cfg.serial_baud_rate = BAUD_RATE_9600;
    cfg.serial_data_bits = DATA_BITS_8;
    cfg.serial_stop_bits = STOP_BITS_1;
    cfg.serial_buffer_size = 64;
    cfg.serial_parity_mode = PARITY_NONE;
    cfg.serial_bit_order = 0;
    cfg.serial_invert_mode = 0;
    cfg.serial_timeout = 1000;
    cfg.is_ext_uart = 0;
#ifdef SENSOR_QS_FX_DRIVER_EXTUART    
    cfg.is_ext_uart = 1;
    cfg.ext_uart_no = SENSOR_DEVICE_QS_FX_DEV_EXT_PORT;
    cfg.port_configure = PORT_CFG_INIT;
#endif

    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = SERIAL_TYPE;
    ioctl_cfg.args = &cfg;
    result = PrivIoctl(sdev->fd, OPE_INT, &ioctl_cfg);

    return result;
}

#ifdef ADD_XIZI_FEATURES
static int PinOpen(void){
    int pin_fd = PrivOpen(SENSOR_DEVICE_QS_FX_PIN_DEV, O_RDWR);
    if (pin_fd < 0) {
        printf("open %s error\n", SENSOR_DEVICE_QS_FX_PIN_DEV);
        return -1;
    }

    //config led pin in board
    struct PinParam pin_parameter;
    memset(&pin_parameter, 0, sizeof(struct PinParam));
    pin_parameter.cmd = GPIO_CONFIG_MODE;
    pin_parameter.pin = SENSOR_DEVICE_QS_FX_PIN_NUMBER;
    pin_parameter.mode = GPIO_CFG_OUTPUT;

    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = PIN_TYPE;
    ioctl_cfg.args =  (void *)&pin_parameter;

    if (0 != PrivIoctl(pin_fd, OPE_CFG, &ioctl_cfg)) {
        printf("ioctl pin fd error %d\n", pin_fd);
        PrivClose(pin_fd);
        return -1;
    }    

    return pin_fd;
}
#endif

/**
 * @description: Read sensor device
 * @param sdev - sensor device pointer
 * @param len - the length of the read data
 * @return get data length
 */
static int SensorDeviceRead(struct SensorDevice *sdev, size_t len)
{
#ifdef ADD_XIZI_FEATURES
    int pin_fd=PinOpen();
    struct PinStat pin_dir;
    pin_dir.pin = SENSOR_DEVICE_QS_FX_PIN_NUMBER;
    pin_dir.val = GPIO_HIGH;
    if (PrivWrite(pin_fd,&pin_dir,0) < 0)   // pull-up pin to configure as tx mode
        return -1;
#endif

    PrivTaskDelay(20);
    if (PrivWrite(sdev->fd, instructions, sizeof(instructions)) < 0)
        return -1;
    PrivTaskDelay(20);

#ifdef ADD_XIZI_FEATURES
    pin_dir.val = GPIO_LOW;
    if (PrivWrite(pin_fd,&pin_dir,0) < 0)   // pull-down pin to configure as rx mode
        return -1;
#endif

    if (PrivRead(sdev->fd, sdev->buffer, len) < 0)
        return -1;
    PrivClose(pin_fd);
    return 0;
}

static struct SensorDone done =
{
    SensorDeviceOpen,
    NULL,
    SensorDeviceRead,
    NULL,
    NULL,
};

/**
 * @description: Init QS-FX sensor and register
 * @return void
 */
static void QsFxInit(void)
{
    qs_fx.name = SENSOR_DEVICE_QS_FX;
    qs_fx.info = &info;
    qs_fx.done = &done;
    qs_fx.status = SENSOR_DEVICE_PASSIVE;

    SensorDeviceRegister(&qs_fx);
}


static struct SensorQuantity qs_fx_wind_direction;

/**
 * @description: Analysis QS-FX wind direction
 * @param quant - sensor quantity pointer
 * @return quantity value
 */
static int32_t ReadWindDirection(struct SensorQuantity *quant)
{
    if (!quant)
        return -1;

    short result;
    if (quant->sdev->done->read != NULL) {
        if (quant->sdev->status == SENSOR_DEVICE_PASSIVE) {
            quant->sdev->done->read(quant->sdev, 7);
            if(Crc16(quant->sdev->buffer,7)!=0x00){
                return -1;
            }
            result = (quant->sdev->buffer[3] << 8) | quant->sdev->buffer[4];

            return (int32_t)result;
        }
        if (quant->sdev->status == SENSOR_DEVICE_ACTIVE) {
            printf("Please set passive mode.\n");
        }
    }else{
        printf("%s don't have read done.\n", quant->name);
    }
    
    return -1;
}

/**
 * @description: Init QS-FX voice quantity and register
 * @return 0
 */
int QsFxWindDirectionInit(void)
{
    QsFxInit();
    
    qs_fx_wind_direction.name = SENSOR_QUANTITY_QS_FX_WINDDIRECTION;
    qs_fx_wind_direction.type = SENSOR_QUANTITY_WINDDIRECTION;
    qs_fx_wind_direction.value.decimal_places = 1;
    qs_fx_wind_direction.value.max_std = 600;
    qs_fx_wind_direction.value.min_std = 0;
    qs_fx_wind_direction.value.last_value = SENSOR_QUANTITY_VALUE_ERROR;
    qs_fx_wind_direction.value.max_value = SENSOR_QUANTITY_VALUE_ERROR;
    qs_fx_wind_direction.value.min_value = SENSOR_QUANTITY_VALUE_ERROR;
    qs_fx_wind_direction.sdev = &qs_fx;
    qs_fx_wind_direction.ReadValue = ReadWindDirection;

    SensorQuantityRegister(&qs_fx_wind_direction);

    return 0;
}
