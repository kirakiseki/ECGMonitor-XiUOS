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
* @brief support imxrt1052 touch function and register to bus framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2022-05-15
*/

#include <board.h>
#include <connect_touch.h>
#include "i2c_touch.h"

#define LCD_HEIGHT  BSP_LCD_X_MAX
#define LCD_WIDTH   BSP_LCD_Y_MAX
#define DEFAULT_NUM 0x0D

uint8_t CTP_CFG_GT911[] =  {
    0x5B,0xE0,0x01,0x10,0x01,0x0A,0x0D,0x00,0x01,0x0A,
    0x28,0x0F,0x5A,0x3C,0x03,0x05,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x87,0x28,0x09,
    0x32,0x34,0x0C,0x08,0x00,0x00,0x00,0x02,0x02,0x1D,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x28,0x55,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,
    0x8D,0x2B,0x00,0x80,0x32,0x00,0x75,0x3A,0x00,0x6C,
    0x43,0x00,0x64,0x4F,0x00,0x64,0x00,0x00,0x00,0x00,
    0xF0,0x4A,0x3A,0xFF,0xFF,0x27,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,0x12,
    0x14,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x26,0x24,0x22,0x21,0x20,0x1F,0x1E,0x1D,
    0x0C,0x0A,0x08,0x06,0x04,0x02,0x00,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x81,0x01
};
uint8_t config[GTP_CONFIG_MAX_LENGTH + GTP_ADDR_LENGTH]
                = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};

int touch_sem = 0;
POINT Pre_Touch_Point;



static int32_t GtpI2cWrite(uint8_t client_addr,uint8_t *buf,int32_t len)
{
    struct i2c_msg msg;
    int32_t ret = -1;
    int32_t retries = 0;

    // GTP_DEBUG_FUNC();
	
    msg.flags = !I2C_M_RD;
    msg.addr  = client_addr;
    msg.len   = len;
    msg.buf   = buf;
    //msg.scl_rate = 300 * 1000;    // for Rockchip, etc
	
    while(retries < 5)
    {
        ret = I2C_Transfer(&msg, 1);
        if (ret == 1)break;
        retries++;
    }
    if((retries >= 5))
    {
        KPrintf("I2C Write: 0x%04X, %d bytes failed, errcode: %d! Process reset.", (((uint16_t)(buf[0] << 8)) | buf[1]), len-2, ret);
        ret = -1;
    }
    return ret;
}
static int32_t GtpI2cRead(uint8_t client_addr, uint8_t *buf, int32_t len)
{
    struct i2c_msg msgs[2];
    int32_t ret = -1;
    int32_t retries = 0;

    msgs[0].flags = !I2C_M_RD;
    msgs[0].addr  = client_addr;
    msgs[0].len   = GTP_ADDR_LENGTH;
    msgs[0].buf   = &buf[0];
	
    msgs[1].flags = I2C_M_RD;
    msgs[1].addr  = client_addr;
    msgs[1].len   = len - GTP_ADDR_LENGTH;
    msgs[1].buf   = &buf[GTP_ADDR_LENGTH];
    
    while(retries < 5)
    {
        ret = I2C_Transfer( msgs, 2);
        if(ret == 2)break;
        retries++;
    }
    if((retries >= 5))
    {
        KPrintf("I2C Read: 0x%04X, %d bytes %d times failed, errcode: %d! Process reset.\n", (((uint16_t)(buf[0] << 8)) | buf[1]), len-2, retries,ret);
        ret = -1;
    }
    return ret;
}
static int32_t gt91xx_Config_Write_Proc()
{
    int32_t ret = -1;

    int32_t i = 0;
    uint8_t check_sum = 0;
    int32_t retry = 0;
    uint8_t cfg_num =0x80FE - 0x8047+1 ;
    
    const uint8_t* cfg_info = CTP_CFG_GT911;
	
    uint8_t cfg_info_len = CFG_GROUP_LEN(CTP_CFG_GT911) ;

    memset(&config[GTP_ADDR_LENGTH], 0, GTP_CONFIG_MAX_LENGTH);
    memcpy(&config[GTP_ADDR_LENGTH], cfg_info,cfg_info_len);


    check_sum = 0;
    for (i = GTP_ADDR_LENGTH; i < cfg_num+GTP_ADDR_LENGTH; i++)
    {
        check_sum += config[i];
    }
    config[ cfg_num+GTP_ADDR_LENGTH] = (~check_sum) + 1; 	//checksum
    config[ cfg_num+GTP_ADDR_LENGTH+1] =  1; 				//refresh ??????
    KPrintf("Driver send config check_sum 0x%x\n",check_sum);
    for (retry = 0; retry < 5; retry++)
    {
        ret = GtpI2cWrite(GTP_ADDRESS, config , cfg_num + GTP_ADDR_LENGTH+2);
        if (ret > 0)
        {
            break;
        }
    }
    return ret;
}

/**
  * @brief   触屏处理函数，轮询或者在触摸中断调用
  * @param 无
  * @retval 无
  */
bool GetTouchEvent(POINT *touch_point,touch_event_t *touch_event)
{

    uint8_t  end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
    uint8_t  point_data[2 + 1 + 8 * GTP_MAX_TOUCH + 1]={GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF};
    uint8_t  touch_num = 0;
    uint8_t  finger = 0;
    static uint16_t pre_touch = 0;

    uint8_t client_addr=GTP_ADDRESS;
    uint8_t* coor_data = NULL;
    int32_t input_x = 0;
    int32_t input_y = 0;
    int32_t input_w = 0;

    int32_t ret = -1;

    ret = GtpI2cRead(client_addr, point_data, 12);//10字节寄存器加2字节地址
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

    if((finger & 0x80) == 0)//判断buffer status位
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
        input_x  = coor_data[1] | (coor_data[2] << 8);	//x坐标
        input_y  = coor_data[3] | (coor_data[4] << 8);	//y坐标
        input_w  = coor_data[5] | (coor_data[6] << 8);	//size
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
        ret = GtpI2cWrite(client_addr, end_cmd, 3);
        if (ret < 0)
        {
            KPrintf("I2C write end_cmd error!\n");
            ret = 0;
        }
    }
	return ret;
}

int32_t GtpReadVersion(void)
{
    int32_t ret = -1;
    uint8_t buf[8] = {GTP_REG_VERSION >> 8, GTP_REG_VERSION & 0xff};

    ret = GtpI2cRead(GTP_ADDRESS, buf, sizeof(buf));
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
    uint8_t end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
    uint8_t opr_buf[6] = {0};
    int32_t ret = 0;

    uint16_t abs_x_max = GTP_MAX_WIDTH;
    uint16_t abs_y_max = GTP_MAX_HEIGHT;
    uint8_t int_trigger_type = GTP_INT_TRIGGER;

    /* config the touch as 480*272 size */
    gt91xx_Config_Write_Proc();

    opr_buf[0] = (uint8_t)((GTP_REG_CONFIG_DATA+1) >> 8);
    opr_buf[1] = (uint8_t)((GTP_REG_CONFIG_DATA+1) & 0xFF);
    
    ret = GtpI2cRead(GTP_ADDRESS, opr_buf, 6);
    if (ret < 0)
    {
        return -1;
    }
    
    abs_x_max = (opr_buf[3] << 8) + opr_buf[2];
    abs_y_max = (opr_buf[5] << 8) + opr_buf[4];
    
    opr_buf[0] = (uint8_t)((GTP_REG_CONFIG_DATA+6) >> 8);
    opr_buf[1] = (uint8_t)((GTP_REG_CONFIG_DATA+6) & 0xFF);
    
    ret = GtpI2cRead(GTP_ADDRESS, opr_buf, 3);
    if (ret < 0)
    {
        return 0;
    }
    int_trigger_type = opr_buf[2] & 0x03;
    
    KPrintf("X_MAX = %d, Y_MAX = %d, TRIGGER = 0x%02x\n",
            abs_x_max,abs_y_max,int_trigger_type);

    ret = GtpI2cWrite(GTP_ADDRESS, end_cmd, 3);
    if (ret < 0)
    {
        KPrintf("I2C write end_cmd error!\n");
        ret = 0;
    }
    return 0;    
}

static uint32 TouchOpen(void *dev)
{
    int32_t ret = -1;

    I2C_Touch_Init();
    ret = GtpReadVersion();
    if(ret < 0)
    {
        KPrintf("gtp read version error\n");
        GtpI2cDeinit();
        return ret;
    }

    ret = GtpGetInfo();
    if(ret < 0)
    {
        KPrintf("gtp read info error\n");
        GtpI2cDeinit();
        return ret;
    }

    touch_sem = KSemaphoreCreate(0);
	if (touch_sem < 0) {
		KPrintf("touch create sem failed .\n");
        GtpI2cDeinit();
		return -1;
	}

    GTP_IRQEnable();
    return ret;
}
static uint32 TouchClose(void *dev)
{
    GTP_IRQDisable();

    KSemaphoreDelete(touch_sem);

    GtpI2cDeinit();
    return 0;
}

static uint32 TouchRead(void *dev, struct BusBlockReadParam *read_param)
{
    uint32  ret  = -1;
    x_err_t result;
    POINT  touch_point; 
	touch_event_t touch_event;

    struct TouchDataStandard *data = (struct TouchDataStandard*)read_param->buffer;
    read_param->read_length = 0;
    result = KSemaphoreObtain(touch_sem, 1000);
    if (EOK == result)
    {
        if(GetTouchEvent(&touch_point, &touch_event))
        {
            data->x = abs(LCD_WIDTH - touch_point.X);
            data->y = abs(LCD_HEIGHT - touch_point.Y);
            data->x = touch_point.X;
            data->y = touch_point.Y;
            g_TouchPadInputSignal = 0;

            read_param->read_length = read_param->size;
            ret = EOK;
        }
        SemReleaseFlag = 0;
    }

    return ret;
}
			
static uint32 TouchConfigure(void *drv, struct BusConfigureInfo *configure_info)
{                           
    return 0;
}

struct TouchDevDone touch_dev_done  =
{
    .open  = TouchOpen,
    .close  = TouchClose,
    .write  =  NONE,
    .read  =  TouchRead
};

static int BoardTouchBusInit(struct TouchBus * touch_bus, struct TouchDriver * touch_driver,const char *bus_name, const char *drv_name)
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
    if (EOK != ret){
        KPrintf("Board_touch_init touchDriverInit error %d\n", ret);
        return -ERROR;
    }

    /*Attach the touch driver to the touch bus*/
    ret = TouchDriverAttachToBus(drv_name, bus_name);
    if (EOK != ret){
        KPrintf("Board_touch_init TouchDriverAttachToBus error %d\n", ret);
        return -ERROR;
    } 

    return ret;
}

/*Attach the touch device to the touch bus*/
static int BoardTouchDevBend(struct TouchHardwareDevice *touch_device, void *param, const char *bus_name, const char *dev_name)
{
    x_err_t ret = EOK;

    ret = TouchDeviceRegister(touch_device, param, dev_name);
    if (EOK != ret){
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

    ret = BoardTouchBusInit(&touch_bus, &touch_driver,TOUCH_BUS_NAME,TOUCH_DRV_NAME);
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
