
// #include "fsl_debug_console.h"	
#include <board.h>
#include "fsl_iomuxc.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_lpi2c.h"

#include "i2c_touch.h"
#include <connect_touch.h>



/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void GTP_I2C_ModeInit(void);

static void I2C_Master_Callback(LPI2C_Type *base, lpi2c_master_handle_t *handle, status_t status, void *userData);
static  uint32_t I2C_Timeout_Callback(uint8_t errorCode);

/*******************************************************************************
 * Variables
 ******************************************************************************/

lpi2c_master_handle_t g_m_handle;
volatile bool g_MasterCompletionFlag = false;
volatile bool g_TouchPadInputSignal = false;
volatile bool SemReleaseFlag = false;
/*******************************************************************************
 * Code
 ******************************************************************************/

/**
  * @brief  对GT91xx芯片进行复位
  * @param  无
  * @retval 无
  */
void GTP_ResetChip(void)
{
	/* 先把RST INT配置为输出模式 */
	gpio_pin_config_t rst_int_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

	GPIO_PinInit(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, &rst_int_config);

	/*初始化GT9157,INT为低电平，则gt9157的设备地址被配置为0xBA*/

	/*复位为低电平，为初始化做准备*/
	GPIO_PinWrite(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, 0U);

	//INT配置成中断输入
	rst_int_config.direction = kGPIO_DigitalInput;
	rst_int_config.outputLogic = 0;
	rst_int_config.interruptMode = kGPIO_IntFallingEdge;

	GPIO_PinInit(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, &rst_int_config);

	/* 使能引脚中断 */  
	GPIO_PortEnableInterrupts(TOUCH_PAD_INT_GPIO, 1U << TOUCH_PAD_INT_GPIO_PIN);
}

/**
* @brief  禁止触摸芯片的中断
* @param  无
* @retval 无
*/
void GTP_IRQDisable(void)
{
	DisableIRQ(GT9xx_PEN_IRQ);
}

/**
* @brief  使能触摸芯片的中断
* @param  无
* @retval 无
*/
void GTP_IRQEnable(void)
{
	
	 IOMUXC_SetPinMux(
      IOMUXC_GPIO_B1_14_GPIO2_IO30,          /* WAKEUP is configured as GPIO5_IO00 */
      0U);                                    /* Software Input On Field: Input Path is determined by functionality */
	
	  IOMUXC_SetPinConfig(
      IOMUXC_GPIO_B1_14_GPIO2_IO30,           
      0x10B0u); 


  gpio_pin_config_t rst_int_config;
   
  //INT配置成中断输入
	rst_int_config.direction = kGPIO_DigitalInput;
	rst_int_config.outputLogic = 0;
	rst_int_config.interruptMode = kGPIO_IntFallingEdge;

	GPIO_PinInit(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, &rst_int_config);

	/* 使能引脚中断 */  
	GPIO_PortEnableInterrupts(TOUCH_PAD_INT_GPIO, 1U << TOUCH_PAD_INT_GPIO_PIN);

  /* 使能中断IRQ */  
	EnableIRQ(GT9xx_PEN_IRQ);
}


/**
* @brief  初始化触摸芯片使用的I2C外设
* @param  无
* @retval 无
*/	
void GTP_I2C_ModeInit(void)
{
	lpi2c_master_config_t masterConfig;	
	
	/*
	 * masterConfig.debugEnable = false;
	 * masterConfig.ignoreAck = false;
	 * masterConfig.pinConfig = kLPI2C_2PinOpenDrain;
	 * masterConfig.baudRate_Hz = 100000U;
	 * masterConfig.busIdleTimeout_ns = 0;
	 * masterConfig.pinLowTimeout_ns = 0;
	 * masterConfig.sdaGlitchFilterWidth_ns = 0;
	 * masterConfig.sclGlitchFilterWidth_ns = 0;
	 */
	LPI2C_MasterGetDefaultConfig(&masterConfig);

	masterConfig.baudRate_Hz = GTP_I2C_BAUDRATE;

	LPI2C_MasterInit(GTP_I2C_MASTER, &masterConfig, LPI2C_MASTER_CLOCK_FREQUENCY);

	LPI2C_MasterTransferCreateHandle(GTP_I2C_MASTER, &g_m_handle, I2C_Master_Callback, NULL);

}

void GtpI2cDeinit(void)
{
	LPI2C_MasterDeinit(GTP_I2C_MASTER);
}

/**
  * @brief   使用IIC读取数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:存放由从机读取的数据的缓冲区指针
  *		@arg NumByteToRead:读取的数据长度
  * @retval  无
  */
uint32_t I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead)
{
	lpi2c_master_transfer_t masterXfer = {0};
	status_t reVal = kStatus_Fail;
    uint32_t i2c_timeout = I2CT_LONG_TIMEOUT;


	/* subAddress = 0x00, data = pBuffer 自从机处接收
		起始信号start + 设备地址slaveaddress(w 写方向) + 子地址subAddress + 
    重复起始信号repeated start + 设备地址slaveaddress(r 读方向) + 
    接收缓冲数据rx data buffer + 停止信号stop */
	masterXfer.slaveAddress = (ClientAddr>>1);
	masterXfer.direction = kLPI2C_Read;
	masterXfer.subaddress = (uint32_t)0;
	masterXfer.subaddressSize = 0;
	masterXfer.data = pBuffer;
	masterXfer.dataSize = NumByteToRead;
	masterXfer.flags = kLPI2C_TransferDefaultFlag;

	/* 复位传输完成标志 */
	g_MasterCompletionFlag = false;

	reVal = LPI2C_MasterTransferNonBlocking(GTP_I2C_MASTER, &g_m_handle, &masterXfer);
	if (reVal != kStatus_Success)
	{
		return 1;
	}
	
	/* 等待传输完成 */
	while (!g_MasterCompletionFlag)
	{
		if((i2c_timeout--) == 0) 
			return I2C_Timeout_Callback(0);
	}
	
	g_MasterCompletionFlag = false;
	
	return 0;
}

/**
  * @brief   使用IIC写入数据
  * @param   
  * 	@arg ClientAddr:从设备地址
  *		@arg pBuffer:缓冲区指针
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
uint32_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite)
{
	lpi2c_master_transfer_t masterXfer = {0};
	status_t reVal = kStatus_Fail;
    uint32_t i2c_timeout = I2CT_LONG_TIMEOUT;


	/* subAddress = 0x00, data = pBuffer 发送至从机
		起始信号start + 设备地址slaveaddress(w 写方向) + 
    发送缓冲数据tx data buffer + 停止信号stop */
  
	masterXfer.slaveAddress = (ClientAddr>>1);
	masterXfer.direction = kLPI2C_Write;
	masterXfer.subaddress = (uint32_t)0;
	masterXfer.subaddressSize = 0;
	masterXfer.data = pBuffer;
	masterXfer.dataSize = NumByteToWrite;
	masterXfer.flags = kLPI2C_TransferDefaultFlag;

	/* 复位传输完成标志 */
	g_MasterCompletionFlag = false;

	reVal = LPI2C_MasterTransferNonBlocking(GTP_I2C_MASTER, &g_m_handle, &masterXfer);
	if (reVal != kStatus_Success)
	{
		return 1;
	}

	/* 等待传输完成 */
	while (!g_MasterCompletionFlag)
	{
    	if((i2c_timeout--) == 0)
			return I2C_Timeout_Callback(1);
	}
	g_MasterCompletionFlag = false;
	
	return 0;

}

/**
  * @brief  IIC等待超时调用本函数输出调试信息
  * @param  None.
  * @retval 返回0xff，表示IIC读取数据失败
  */
static  uint32_t I2C_Timeout_Callback(uint8_t errorCode)
{
	/* Block communication and all processes */
	KPrintf("I2C timeout!errorCode = %d\n",errorCode);

	return 0xFF;
}

/**
* @brief  I2C外设传输完成的回调函数
* @param  无
* @retval 无
*/
static void I2C_Master_Callback(LPI2C_Type *base, lpi2c_master_handle_t *handle, status_t status, void *userData)
{		
    /* 接收到kStatus_Success标志后，
       设置g_MasterCompletionFlag标志表明传输成功 */
    if (status == kStatus_Success)
    {
        g_MasterCompletionFlag = true;
    }
}



/**
* @brief  触摸引脚及芯片初始化
* @param  无
* @retval 无
*/
void I2C_Touch_Init(void)
{

//  /* 初始化I2C外设工作模式 */
  GTP_I2C_ModeInit(); 
  
  /* 复位触摸芯片，配置地址 */
  GTP_ResetChip();
}


/***************************中断相关******************************/
/**
* @brief  触摸芯片TOUCH_PAD_INT_GPIO_PIN引脚的中断服务函数
* @param  无
* @retval 无
*/
//void TOUCH_PAD_IRQHANDLER(void)
extern int touch_sem;
void GT9xx_PEN_IRQHandler(int irqn, void *arg)
{
	DisableIRQ(GT9xx_PEN_IRQ);
    /* 确认是触摸芯片的中断 */
    if(GPIO_GetPinsInterruptFlags(TOUCH_PAD_INT_GPIO) & 1U << TOUCH_PAD_INT_GPIO_PIN)
    {
       /* 清除中断标志 */
      GPIO_PortClearInterruptFlags(TOUCH_PAD_INT_GPIO, 1U << TOUCH_PAD_INT_GPIO_PIN);
      /* 切换触摸输入状态标志 */
      g_TouchPadInputSignal = true;
	  if(!SemReleaseFlag) 
	  {
		KSemaphoreAbandon(touch_sem);
		SemReleaseFlag = true;
	  }
    }
	EnableIRQ(GT9xx_PEN_IRQ);
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
DECLARE_HW_IRQ(GT9xx_PEN_IRQ, GT9xx_PEN_IRQHandler, NONE);


/**
 * i2c_transfer - execute a single or combined I2C message
 * @msgs: One or more messages to execute before STOP is issued to
 *	terminate the operation; each message begins with a START.
 * @num: Number of messages to be executed.
 */
int I2C_Transfer( struct i2c_msg *msgs,int num)
{
	int im = 0;
	int ret = 0;

	//GTP_DEBUG_FUNC();

	for (im = 0; ret == 0 && im != num; im++)
	{
		if ((msgs[im].flags&I2C_M_RD))																//根据flag判断是读数据还是写数据
		{
			ret = I2C_ReadBytes(msgs[im].addr, msgs[im].buf, msgs[im].len);		//IIC读取数据
		} else
		{
			ret = I2C_WriteBytes(msgs[im].addr,  msgs[im].buf, msgs[im].len);	//IIC写入数据
		}
	}

	if(ret)
		return ret;

	return im;   													//正常完成的传输结构个数
}












/*********************************************END OF FILE**********************/
