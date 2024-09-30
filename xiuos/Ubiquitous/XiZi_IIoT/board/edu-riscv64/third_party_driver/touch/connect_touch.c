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
* @file connect_touch.c
* @brief support edu-riscv64 touch function and register to bus framework
* @version 1.0
* @author AIIT XiUOS Lab
* @date 2022-10-17
*/

#include <stdbool.h>
#include <board.h>
#include <connect_touch.h>
#include <bus.h>
#include <gpiohs.h>
#include <fpioa.h>
#include "gsl2038firmware.h"


struct Finger {
    uint8_t fingerID;
    uint32_t x;
    uint32_t y;
};
struct Touch_event {
    uint8_t NBfingers;
    struct Finger fingers[5];
};

#define DEFAULT_NUM 0x0D
#define TOUCH_ADDRESS 0x40
volatile bool SemReleaseFlag = 0;

static struct Bus* i2c_bus = NONE;
static struct Bus* pin_bus = NONE;
int touch_sem = 0;

#define DATA_REG 0x80
#define STATUS_REG 0xE0

/* HERE WE IMPLEMENT I2C READING AND WRITING FROM SENSOR */
/* write sensor register data */
static x_err_t WriteReg(struct HardwareDev* dev, char* buf, int len)
{
    struct BusBlockWriteParam write_param;
    write_param.pos = 0;
    write_param.size = len;
    write_param.buffer = (void*)buf;

    /* use I2C device API transfer data */
    return BusDevWriteData(dev, &write_param);
}

/* read sensor register data */
static x_err_t ReadRegs(struct HardwareDev* dev, uint8 len, uint8* buf)
{
    struct BusBlockReadParam read_param;
    read_param.pos = 0;
    read_param.buffer = (void*)buf;
    read_param.read_length = len;
    read_param.size = len;

    /* use I2C device API transfer data */
    return BusDevReadData(dev, &read_param);
}

// not used in polling mode
static void touch_pin_irqhandler(void* arg)
{
    //KPrintf("int hdr working.\n");
    if (!SemReleaseFlag)
    {
        KSemaphoreAbandon(touch_sem);
        SemReleaseFlag = true;
    }
}

int32_t touch_irq_init() 
{
    int32_t ret = -ERROR;
    struct PinParam pin_param;
    struct BusConfigureInfo pin_configure_info;

    pin_bus = PinBusInitGet();

    pin_configure_info.configure_cmd = OPE_CFG;
    pin_configure_info.private_data = (void*)&pin_param;

    pin_param.cmd = GPIO_CONFIG_MODE;
    pin_param.pin = BSP_TOUCH_TP_INT;
    pin_param.mode = GPIO_CFG_INPUT;
    ret = BusDrvConfigure(pin_bus->owner_driver, &pin_configure_info);
    if (ret != EOK) {
        KPrintf("config pin_param  %d input failed!\n", pin_param.pin);
        return -ERROR;
    }

    pin_param.cmd = GPIO_IRQ_REGISTER;
    pin_param.pin = BSP_TOUCH_TP_INT;
    pin_param.irq_set.irq_mode = GPIO_IRQ_EDGE_BOTH;
    pin_param.irq_set.hdr = touch_pin_irqhandler;
    pin_param.irq_set.args = NONE;
    ret = BusDrvConfigure(pin_bus->owner_driver, &pin_configure_info);
    if (ret != EOK) {
        KPrintf("register pin_param  %d  irq failed!\n", pin_param.pin);
        return -ERROR;
    }

    pin_param.cmd = GPIO_IRQ_DISABLE;
    pin_param.pin = BSP_TOUCH_TP_INT;
    ret = BusDrvConfigure(pin_bus->owner_driver, &pin_configure_info);
    if (ret != EOK) {
        KPrintf("disable pin_param  %d  irq failed!\n", pin_param.pin);
        return -ERROR;
    }

    // 4. enable interuption
    pin_param.cmd = GPIO_IRQ_ENABLE;
    pin_param.pin = BSP_TOUCH_TP_INT;
    ret = BusDrvConfigure(pin_bus->owner_driver, &pin_configure_info);
    if (ret != EOK) {
        KPrintf("enable pin_param  %d  irq failed!\n", pin_param.pin);
        return -ERROR;
    }

    return EOK;
}

int32_t I2cTouchInit() {
    // using static bus information
    int32_t ret = -1;
    /* find I2C device and get I2C handle */
    i2c_bus = BusFind(I2C_BUS_NAME_1);
    if (NONE == i2c_bus) {
        KPrintf("MCU can't find %s bus!\n", I2C_BUS_NAME_1);
        return -ERROR;
    }
    else {
        KPrintf("MCU find %s bus!\n", I2C_BUS_NAME_1);
    }
    i2c_bus->owner_haldev = BusFindDevice(i2c_bus, I2C_1_DEVICE_NAME_0);
    i2c_bus->owner_driver = BusFindDriver(i2c_bus, I2C_DRV_NAME_1);

    if (i2c_bus->match(i2c_bus->owner_driver, i2c_bus->owner_haldev)) {
        KPrintf("i2c match drv %s  %p dev %s %p error\n",
            I2C_DRV_NAME_1, i2c_bus->owner_driver,
            I2C_1_DEVICE_NAME_0, i2c_bus->owner_haldev);
        return -ERROR;
    }
    else {
        KPrintf("MCU successfully! write %p read %p\n",
            i2c_bus->owner_haldev->dev_done->write,
            i2c_bus->owner_haldev->dev_done->read);
    }
    struct BusConfigureInfo i2c_configure_info;

    // memset(&i2c_configure_info, 0, sizeof(struct BusConfigureInfo));
    i2c_configure_info.configure_cmd = OPE_INT;
    uint16 i2c_address = TOUCH_ADDRESS;
    i2c_configure_info.private_data = (void*)&i2c_address;
    BusDrvConfigure(i2c_bus->owner_driver, &i2c_configure_info);

    // 3. init interruption
    return touch_irq_init();
}

void loadfw(struct HardwareDev *dev)
{
    uint8_t addr;
    uint8_t Wrbuf[5];
    size_t source_len = sizeof(GSL2038_FW) / sizeof(struct fw_data);
    
    for (size_t source_line = 0; source_line < source_len; source_line++) {
        // addr = GSL2038_FW[source_line].offset;
        memset(Wrbuf, 0 , 5);
        Wrbuf[0] = GSL2038_FW[source_line].offset;
        Wrbuf[1] = (char)(GSL2038_FW[source_line].val & 0x000000ff);
        Wrbuf[2] = (char)((GSL2038_FW[source_line].val & 0x0000ff00) >> 8);
        Wrbuf[3] = (char)((GSL2038_FW[source_line].val & 0x00ff0000) >> 16);
        Wrbuf[4] = (char)((GSL2038_FW[source_line].val & 0xff000000) >> 24);

        WriteReg(dev, Wrbuf,5);
    }
}

void reset()
{
    uint8_t REG[6] = {STATUS_REG, 0xE4, 0xbc, 0xbd, 0xbe, 0xbf};
    uint8_t DATA[6] = {0x88, 0x04, 0x00, 0x00, 0x00, 0x00};
    char reg_data[2];

    int i;
    for (i = 0; i < sizeof(REG); ++i)
    {
        // WriteReg(i2c_bus->owner_haldev, &REG[i],1);
        // WriteReg(i2c_bus->owner_haldev, &DATA[i],1);
        reg_data[0] = REG[i];
        reg_data[1] = DATA[i];
        WriteReg(i2c_bus->owner_haldev, reg_data,2);
        MdelayKTask(10);
    }
}

void startchip()
{
    char reg_data[] = {0xE0, 0x00};

    WriteReg(i2c_bus->owner_haldev, reg_data,2); // Registre
}

static uint32 TouchOpen(void* dev)
{
    int32_t ret = 0;

    I2cTouchInit();

    reset();
    loadfw(i2c_bus->owner_haldev);
    reset();
    startchip();

    touch_sem = KSemaphoreCreate(0);
    if (touch_sem < 0) {
        KPrintf("touch create sem failed .\n");
        return -1;
    }

    return ret;
}

static uint32 TouchClose(void* dev)
{   
    int32_t ret = -ERROR;
    struct PinParam pin_param;
    struct BusConfigureInfo pin_configure_info;

    pin_configure_info.configure_cmd = OPE_CFG;
    pin_configure_info.private_data = (void*)&pin_param;

    pin_param.cmd = GPIO_IRQ_DISABLE;
    pin_param.pin = BSP_TOUCH_TP_INT;
    ret = BusDrvConfigure(pin_bus->owner_driver, &pin_configure_info);
    if (ret != EOK) {
        KPrintf("disable pin_param  %d  irq failed!\n", pin_param.pin);
        return -ERROR;
    }

    pin_param.cmd = GPIO_IRQ_FREE;
    pin_param.pin = BSP_TOUCH_TP_INT;
    ret = BusDrvConfigure(pin_bus->owner_driver, &pin_configure_info);
    if (ret != EOK) {
        KPrintf("register pin_param  %d  irq failed!\n", pin_param.pin);
        return -ERROR;
    }

    KSemaphoreDelete(touch_sem);

    return 0;
}

static uint32 TouchRead(void* dev, struct BusBlockReadParam* read_param)
{
    uint32  ret = -1;
    x_err_t result;
    uint8_t TOUCHRECDATA[24] = {0};
    struct Touch_event ts_event;
    char status_reg = 0x80;

    struct TouchDataStandard* data = (struct TouchDataStandard*)read_param->buffer;
    
    result = KSemaphoreObtain(touch_sem, 100);
    // if (EOK == result)
    // {
        memset(TOUCHRECDATA, 0, 24);
        memset(&ts_event, 0, sizeof(struct Touch_event));

        WriteReg(i2c_bus->owner_haldev, &status_reg, 1);
        ReadRegs(i2c_bus->owner_haldev, 24, TOUCHRECDATA);
        ts_event.NBfingers = TOUCHRECDATA[0];

        for (int i = 0; i < ts_event.NBfingers; i++)
        {
            ts_event.fingers[i].x = ((((uint32_t)TOUCHRECDATA[(i * 4) + 5]) << 8) | (uint32_t)TOUCHRECDATA[(i * 4) + 4]) & 0x00000FFF; // 12 bits of X coord
            ts_event.fingers[i].y = ((((uint32_t)TOUCHRECDATA[(i * 4) + 7]) << 8) | (uint32_t)TOUCHRECDATA[(i * 4) + 6]) & 0x00000FFF;
            uint32_t pos_y = 320 > ts_event.fingers[i].x *LCD_SIZE/TOUCH_WIDTH?320 - ts_event.fingers[i].x *LCD_SIZE/TOUCH_WIDTH:0;
            ts_event.fingers[i].x = ts_event.fingers[i].y *LCD_SIZE/TOUCH_HEIGHT>6?ts_event.fingers[i].y *LCD_SIZE/TOUCH_HEIGHT-6:0;
            ts_event.fingers[i].y = pos_y;
            ts_event.fingers[i].fingerID = (uint32_t)TOUCHRECDATA[(i * 4) + 7] >> 4; // finger that did the touch
            // printf("fingers[%d] x %d y %d id %d\n",i,ts_event.fingers[i].x,ts_event.fingers[i].y,ts_event.fingers[i].fingerID);
        }

        data->x = ts_event.fingers[ts_event.NBfingers - 1].x;
        data->y = ts_event.fingers[ts_event.NBfingers - 1].y;
        read_param->read_length = ts_event.NBfingers;
        SemReleaseFlag = 0;
    // }

    return ret;
}

static uint32 TouchConfigure(void* drv, struct BusConfigureInfo* configure_info)
{
    return 0;
}

struct TouchDevDone touch_dev_done =
{
    .open = TouchOpen,
    .close = TouchClose,
    .write = NONE,
    .read = TouchRead
};

/* BUS, DRIVER, DEVICE INIT */
// register bus and driver
static int BoardTouchBusInit(struct TouchBus* touch_bus, struct TouchDriver* touch_driver, const char* bus_name, const char* drv_name)
{
    x_err_t ret = EOK;

    /*Init the touch bus */
    ret = TouchBusInit(touch_bus, bus_name);
    if (EOK != ret) {
        KPrintf("Board_touch_init touchBusInit error %d\n", ret);
        return -ERROR;
    }

    /*Init the touch driver*/
    ret = TouchDriverInit(touch_driver, drv_name);
    if (EOK != ret) {
        KPrintf("Board_touch_init touchDriverInit error %d\n", ret);
        return -ERROR;
    }

    /*Attach the touch driver to the touch bus*/
    ret = TouchDriverAttachToBus(drv_name, bus_name);
    if (EOK != ret) {
        KPrintf("Board_touch_init TouchDriverAttachToBus error %d\n", ret);
        return -ERROR;
    }

    return ret;
}

/*Attach the touch device to the touch bus*/
static int BoardTouchDevBend(struct TouchHardwareDevice* touch_device, void* param, const char* bus_name, const char* dev_name)
{
    x_err_t ret = EOK;

    ret = TouchDeviceRegister(touch_device, param, dev_name);
    if (EOK != ret) {
        KPrintf("TouchDeviceRegister  device %s error %d\n", dev_name, ret);
        return -ERROR;
    }

    ret = TouchDeviceAttachToBus(dev_name, bus_name);
    if (EOK != ret) {
        KPrintf("TouchDeviceAttachToBus  device %s error %d\n", dev_name, ret);
        return -ERROR;
    }

    return ret;
}

// init bus, driver and device needed.
// stored in touch_bus, touch_driver and touch_dev
int HwTouchInit(void)
{
    x_err_t ret = EOK;

    static struct TouchBus touch_bus;
    static struct TouchDriver touch_driver;
    static struct TouchHardwareDevice touch_dev;

    memset(&touch_bus, 0, sizeof(struct TouchBus));
    memset(&touch_driver, 0, sizeof(struct TouchDriver));
    memset(&touch_dev, 0, sizeof(struct TouchHardwareDevice));

    touch_driver.configure = TouchConfigure;

    ret = BoardTouchBusInit(&touch_bus, &touch_driver, TOUCH_BUS_NAME, TOUCH_DRV_NAME);
    if (EOK != ret) {
        return -ERROR;
    }

    touch_dev.dev_done = &touch_dev_done;

    ret = BoardTouchDevBend(&touch_dev, NONE, TOUCH_BUS_NAME, TOUCH_DEVICE_NAME);
    if (EOK != ret) {
        KPrintf("board_touch_Init error ret %u\n", ret);
        return -ERROR;
    }

    return EOK;
}
