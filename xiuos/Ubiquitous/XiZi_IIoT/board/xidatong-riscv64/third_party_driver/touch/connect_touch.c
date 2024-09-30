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
* @brief support xidatong-riscv64 touch function and register to bus framework
* @version 1.0
* @author AIIT XiUOS Lab
* @date 2022-05-15
*/

#include <stdbool.h>
#include <board.h>
#include <connect_touch.h>
#include <bus.h>
#include <gpiohs.h>
#include <fpioa.h>

// #define LCD_HEIGHT  BSP_LCD_X_MAX
// #define LCD_WIDTH   BSP_LCD_Y_MAX
#define DEFAULT_NUM 0x0D

volatile bool SemReleaseFlag = 0;

static struct Bus* i2c_bus = NONE;
static struct Bus* pin_bus = NONE;
int touch_sem = 0;
POINT Pre_Touch_Point;

/* HERE WE IMPLEMENT I2C READING AND WRITING FROM SENSOR */
/* write sensor register data */
static x_err_t WriteReg(struct HardwareDev* dev, uint8 len, uint8* buf)
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

/**
 * i2c_transfer - execute a single I2C message
 * @msgs: One or more messages to execute before STOP is issued to
 *	terminate the operation; each message begins with a START.
 */
int I2C_Transfer(struct i2c_msg* msg)
{
    int16 ret = 0;

    if (msg->flags & I2C_M_RD)																//根据flag判断是读数据还是写数据
    {
        ret = ReadRegs(i2c_bus->owner_haldev, msg->len, msg->buf);		//IIC读取数据
    }
    else
    {
        ret = WriteReg(i2c_bus->owner_haldev, msg->len, msg->buf);	//IIC写入数据
    }  													//正常完成的传输结构个数

    return ret;
}

static int32_t GtpI2cWrite(uint8_t* buf, int32_t len)
{
    struct i2c_msg msg;
    int32_t ret = -1;
    int32_t retries = 0;

    msg.flags = !I2C_M_RD;
    msg.len = len;
    msg.buf = buf;
    //msg.scl_rate = 300 * 1000;    // for Rockchip, etc
    while (retries < 5)
    {
        ret = I2C_Transfer(&msg);
        if (ret == 1) { break; }
        retries++;
    }
    if (retries >= 5)
    {
        KPrintf("I2C Write: 0x%04X, %d bytes failed, errcode: %d! Process reset.", (((uint16_t)(buf[0] << 8)) | buf[1]), len - 2, ret);
        ret = -1;
    }
    return ret;
}
static int32_t GtpI2cRead(uint8_t* buf, int32_t len)
{
    struct i2c_msg msgs[2];
    int32_t ret = -1;
    int32_t retries = 0;

    // write reading addr.
    msgs[0].flags = !I2C_M_RD;
    msgs[0].len = GTP_ADDR_LENGTH;
    msgs[0].buf = buf;

    // read data at addr sended.
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = len - GTP_ADDR_LENGTH;
    msgs[1].buf = &buf[GTP_ADDR_LENGTH];

    while (retries < 5)
    {
        ret = I2C_Transfer(&msgs[0]);
        ret += I2C_Transfer(&msgs[1]);
        if (ret == 2)break;
        retries++;
    }
    if (retries >= 5)
    {
        KPrintf("I2C Read: 0x%04X, %d bytes %d times failed, errcode: %d! Process reset.\n", (((uint16_t)(buf[0] << 8)) | buf[1]), len - 2, retries, ret);
        ret = -1;
    }
    return ret;
}

/* HERE WE IMPLEMENT TOUCH INIT */
int32_t GtpReadVersion(void)
{
    int32_t ret = -1;
    uint8_t buf[8] = { GTP_REG_VERSION >> 8, GTP_REG_VERSION & 0xff };

    ret = GtpI2cRead(buf, sizeof(buf));
    if (ret < 0)
    {
        KPrintf("GTP read version failed.\n");
        return ret;
    }

    if (buf[5] == 0x00)
    {
        KPrintf("IC1 Version: %c%c%c_%02x%02x\n", buf[2], buf[3], buf[4], buf[7], buf[6]);
    }
    else
    {
        KPrintf("IC2 Version: %c%c%c%c_%02x%02x\n", buf[2], buf[3], buf[4], buf[5], buf[7], buf[6]);
    }
    return ret;
}

static int32_t GtpGetInfo(void)
{
    uint8_t end_cmd[3] = { GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0 };
    uint8_t opr_buf[6] = { 0 };
    int32_t ret = 0;

    uint16_t abs_x_max = GTP_MAX_WIDTH;
    uint16_t abs_y_max = GTP_MAX_HEIGHT;
    uint8_t int_trigger_type = GTP_INT_TRIGGER;

    opr_buf[0] = (uint8_t)((GTP_REG_CONFIG_DATA + 1) >> 8);
    opr_buf[1] = (uint8_t)((GTP_REG_CONFIG_DATA + 1) & 0xFF);

    if (GtpI2cRead(opr_buf, 6) < 0)
    {
        return -1;
    }

    abs_x_max = (opr_buf[3] << 8) + opr_buf[2];
    abs_y_max = (opr_buf[5] << 8) + opr_buf[4];

    opr_buf[0] = (uint8_t)((GTP_REG_CONFIG_DATA + 6) >> 8);
    opr_buf[1] = (uint8_t)((GTP_REG_CONFIG_DATA + 6) & 0xFF);

    if (GtpI2cRead(opr_buf, 3) < 0)
    {
        return 0;
    }

    int_trigger_type = opr_buf[2] & 0x03;
    KPrintf("X_MAX = %d, Y_MAX = %d, TRIGGER = 0x%02x\n",
        abs_x_max, abs_y_max, int_trigger_type);

    if (GtpI2cWrite(end_cmd, 3) < 0)
    {
        KPrintf("I2C write end_cmd error!\n");
        ret = 0;
    }
    return 0; ;
}

// not used in polling mode
static void GT9xx_PEN_IRQHandler(void* arg)
{
    //KPrintf("int hdr working.\n");
    if (!SemReleaseFlag)
    {
        KSemaphoreAbandon(touch_sem);
        SemReleaseFlag = true;
    }
}

int32_t GT9xx_INT_INIT() {
    int32_t ret = -ERROR;

    pin_bus = PinBusInitGet();

    struct PinParam pin_param;
    struct BusConfigureInfo pin_configure_info;

    pin_configure_info.configure_cmd = OPE_CFG;
    pin_configure_info.private_data = (void*)&pin_param;

    pin_param.cmd = GPIO_CONFIG_MODE;
    pin_param.pin = BSP_TOUCH_TP_INT;
    pin_param.mode = GPIO_CFG_INPUT_PULLUP;
    ret = BusDrvConfigure(pin_bus->owner_driver, &pin_configure_info);
    if (ret != EOK) {
        KPrintf("config pin_param  %d input failed!\n", pin_param.pin);
        return -ERROR;
    }

    pin_param.cmd = GPIO_IRQ_REGISTER;
    pin_param.pin = BSP_TOUCH_TP_INT;
    pin_param.irq_set.irq_mode = GPIO_IRQ_EDGE_FALLING;
    pin_param.irq_set.hdr = GT9xx_PEN_IRQHandler;
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

int32_t I2C_Touch_Init() {
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
    uint16 i2c_address = GTP_ADDRESS >> 1;
    i2c_configure_info.private_data = (void*)&i2c_address;
    BusDrvConfigure(i2c_bus->owner_driver, &i2c_configure_info);

    // 3. init interruption
    return GT9xx_INT_INIT();
}


/* HERE WE IMPLEMENT GET COORDINATE FUNCTION */
/**
  * @brief   触屏处理函数，轮询或者在触摸中断调用
  * @param 无
  * @retval 无
  */
bool GetTouchEvent(POINT* touch_point, touch_event_t* touch_event)
{

    uint8_t  end_cmd[3] = { GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0 };
    uint8_t  point_data[2 + 1 + 8 * GTP_MAX_TOUCH + 1] = { GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF };
    uint8_t  touch_num = 0;
    uint8_t  finger = 0;
    static uint16_t pre_touch = 0;

    uint8_t* coor_data = NULL;
    int32_t input_x = 0;
    int32_t input_y = 0;
    int32_t input_w = 0;

    int32_t ret = -1;

    ret = GtpI2cRead(point_data, 12);//10字节寄存器加2字节地址
    if (ret < 0)
    {
        KPrintf("I2C transfer error. errno:%d\n ", ret);
        return 0;
    }

    finger = point_data[GTP_ADDR_LENGTH];//状态寄存器数据

    if (finger == 0x00)		//没有数据，退出
    {
        ret = 0;
        goto exit_work_func;
    }

    if ((finger & 0x80) == 0)//判断buffer status位
    {
        ret = 0;
        goto exit_work_func;//坐标未就绪，数据无效
    }

    touch_num = finger & 0x0f;//坐标点数
    if (touch_num > GTP_MAX_TOUCH)
    {
        ret = 0;
        goto exit_work_func;//大于最大支持点数，错误退出
    }

    if (touch_num)
    {
        coor_data = &point_data[0 * 8 + 3];
        input_x = coor_data[1] | (coor_data[2] << 8);	//x坐标
        input_y = coor_data[3] | (coor_data[4] << 8);	//y坐标
        input_w = coor_data[5] | (coor_data[6] << 8);	//size
        touch_point->X = input_x;
        touch_point->Y = input_y;
        *touch_event = kTouch_Down;
        Pre_Touch_Point = *touch_point;

    }
    else if (pre_touch)		//touch_ num=0 且pre_touch！=0
    {
        *touch_point = Pre_Touch_Point;
        *touch_event = kTouch_Up;
        Pre_Touch_Point.X = -1;
        Pre_Touch_Point.Y = -1;
    }
    pre_touch = touch_num;

exit_work_func:
    {
        ret = GtpI2cWrite(end_cmd, 3);
        if (ret < 0)
        {
            KPrintf("I2C write end_cmd error!\n");
            ret = 0;
        }
    }
    return ret;
}

static uint32 TouchOpen(void* dev)
{
    int32_t ret = -1;

    I2C_Touch_Init();
    ret = GtpReadVersion();
    if (ret < 0)
    {
        KPrintf("gtp read version error\n");
        return ret;
    }

    ret = GtpGetInfo();
    if (ret < 0)
    {
        KPrintf("gtp read info error\n");
        return ret;
    }

    touch_sem = KSemaphoreCreate(0);
    if (touch_sem < 0) {
        KPrintf("touch create sem failed .\n");
        return -1;
    }

    return ret;
}

static uint32 TouchClose(void* dev)
{
    KSemaphoreDelete(touch_sem);
    return 0;
}

static uint32 TouchRead(void* dev, struct BusBlockReadParam* read_param)
{
    uint32  ret = -1;
    x_err_t result;
    POINT  touch_point;
    touch_point.X = -1;
    touch_point.Y = -1;
    touch_event_t touch_event;

    struct TouchDataStandard* data = (struct TouchDataStandard*)read_param->buffer;
    read_param->read_length = 0;
    result = KSemaphoreObtain(touch_sem, 100);
    if (GetTouchEvent(&touch_point, &touch_event))
    {
        data->x = touch_point.X;
        data->y = touch_point.Y;

        read_param->read_length = read_param->size;
        ret = EOK;
    }
    SemReleaseFlag = 0;

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
