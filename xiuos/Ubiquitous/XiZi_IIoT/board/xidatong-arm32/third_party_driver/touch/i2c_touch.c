
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
uint32_t i2c_lockup_cnt = 0;
/*******************************************************************************
 * Code
 ******************************************************************************/

/**
  * @brief  ��GT91xxоƬ���и�λ
  * @param  ��
  * @retval ��
  */
void GTP_ResetChip(void)
{
	/* �Ȱ�RST INT����Ϊ���ģʽ */
	gpio_pin_config_t rst_int_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};

	GPIO_PinInit(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, &rst_int_config);

	/*��ʼ��GT9157,INTΪ�͵�ƽ����gt9157���豸��ַ������Ϊ0xBA*/

	/*��λΪ�͵�ƽ��Ϊ��ʼ����׼��*/
	GPIO_PinWrite(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, 0U);

	//INT���ó��ж�����
	rst_int_config.direction = kGPIO_DigitalInput;
	rst_int_config.outputLogic = 0;
	rst_int_config.interruptMode = kGPIO_IntFallingEdge;

	GPIO_PinInit(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, &rst_int_config);

	/* ʹ�������ж� */  
	GPIO_PortEnableInterrupts(TOUCH_PAD_INT_GPIO, 1U << TOUCH_PAD_INT_GPIO_PIN);
}

/**
* @brief  ��ֹ����оƬ���ж�
* @param  ��
* @retval ��
*/
void GTP_IRQDisable(void)
{
	DisableIRQ(GT9xx_PEN_IRQ);
}

/**
* @brief  ʹ�ܴ���оƬ���ж�
* @param  ��
* @retval ��
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
   
  //INT���ó��ж�����
	rst_int_config.direction = kGPIO_DigitalInput;
	rst_int_config.outputLogic = 0;
	rst_int_config.interruptMode = kGPIO_IntFallingEdge;

	GPIO_PinInit(TOUCH_PAD_INT_GPIO, TOUCH_PAD_INT_GPIO_PIN, &rst_int_config);

	/* ʹ�������ж� */  
	GPIO_PortEnableInterrupts(TOUCH_PAD_INT_GPIO, 1U << TOUCH_PAD_INT_GPIO_PIN);

  /* ʹ���ж�IRQ */  
	EnableIRQ(GT9xx_PEN_IRQ);
}


/**
* @brief  ��ʼ������оƬʹ�õ�I2C����
* @param  ��
* @retval ��
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

void I2CLockupRecover(void) {
	IOMUXC_SetPinMux(TOUCH_PAD_RECOVER_SCL_IOMUXC, 0U);
	IOMUXC_SetPinMux(TOUCH_PAD_RECOVER_SDA_IOMUXC, 0U);

	const gpio_pin_config_t rec_pin_config = {
		.direction = kGPIO_DigitalOutput,
		.outputLogic = 1U,
	};

	GPIO_PinInit(TOUCH_PAD_RECOVER_SCL_GPIO, TOUCH_PAD_RECOVER_SCL_GPIO_PIN, &rec_pin_config);
	GPIO_PinInit(TOUCH_PAD_RECOVER_SDA_GPIO, TOUCH_PAD_RECOVER_SDA_GPIO_PIN, &rec_pin_config);

	uint32_t primask = DisableGlobalIRQ();
	// recover scl
	for (uint32_t i = 0; i < 0xfffff; ++i) {}
	for (uint32_t i = 0U; i < I2C_RECOVER_NUM_CLOCKS; ++i) {
		for (uint32_t i = 0; i < 0xfffff; ++i) {}
		GPIO_PinWrite(TOUCH_PAD_RECOVER_SCL_GPIO, TOUCH_PAD_RECOVER_SCL_GPIO_PIN, 0U);
		for (uint32_t i = 0; i < 0xfffff; ++i) {}
		GPIO_PinWrite(TOUCH_PAD_RECOVER_SCL_GPIO, TOUCH_PAD_RECOVER_SCL_GPIO_PIN, 1U);
	}
	GPIO_PinWrite(TOUCH_PAD_RECOVER_SDA_GPIO, TOUCH_PAD_RECOVER_SDA_GPIO_PIN, 1U);
	EnableGlobalIRQ(primask);

	// reset pin to scl
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL, 1U);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL, 0xD8B0u);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA, 1U);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA, 0xD8B0u);

	i2c_lockup_cnt = 0;
}

/**
  * @brief   ʹ��IIC��ȡ����
  * @param   
  * 	@arg ClientAddr:���豸��ַ
  *		@arg pBuffer:����ɴӻ���ȡ�����ݵĻ�����ָ��
  *		@arg NumByteToRead:��ȡ�����ݳ���
  * @retval  ��
  */
uint32_t I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead)
{
	lpi2c_master_transfer_t masterXfer = { 0 };
	status_t reVal = kStatus_Fail;
    uint32_t i2c_timeout = I2CT_LONG_TIMEOUT;
	/* subAddress = 0x00, data = pBuffer �Դӻ�������
		��ʼ�ź�start + �豸��ַslaveaddress(w д����) + �ӵ�ַsubAddress + 
    �ظ���ʼ�ź�repeated start + �豸��ַslaveaddress(r ������) + 
    ���ջ�������rx data buffer + ֹͣ�ź�stop */
	masterXfer.slaveAddress = (ClientAddr>>1);
	masterXfer.direction = kLPI2C_Read;
	masterXfer.subaddress = (uint32_t)0;
	masterXfer.subaddressSize = 0;
	masterXfer.data = pBuffer;
	masterXfer.dataSize = NumByteToRead;
	masterXfer.flags = kLPI2C_TransferDefaultFlag;

	/* ��λ������ɱ�־ */
	g_MasterCompletionFlag = false;
	reVal = LPI2C_MasterTransferNonBlocking(GTP_I2C_MASTER, &g_m_handle, &masterXfer);
	if (reVal != kStatus_Success)
	{
		return 1;
	}
	/* �ȴ�������� */
	while (!g_MasterCompletionFlag)
	{
		if ((i2c_timeout--) == 0)
			return I2C_Timeout_Callback(0);
	}
	
	g_MasterCompletionFlag = false;
	return 0;
}

/**
  * @brief   ʹ��IICд������
  * @param   
  * 	@arg ClientAddr:���豸��ַ
  *		@arg pBuffer:������ָ��
  *     @arg NumByteToWrite:д���ֽ���
  * @retval  ��
  */
uint32_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite)
{
	lpi2c_master_transfer_t masterXfer = { 0 };
	status_t reVal = kStatus_Fail;
    uint32_t i2c_timeout = I2CT_LONG_TIMEOUT;


	/* subAddress = 0x00, data = pBuffer �������ӻ�
		��ʼ�ź�start + �豸��ַslaveaddress(w д����) + 
    ���ͻ�������tx data buffer + ֹͣ�ź�stop */

	masterXfer.slaveAddress = (ClientAddr >> 1);
	masterXfer.direction = kLPI2C_Write;
	masterXfer.subaddress = (uint32_t)0;
	masterXfer.subaddressSize = 0;
	masterXfer.data = pBuffer;
	masterXfer.dataSize = NumByteToWrite;
	masterXfer.flags = kLPI2C_TransferDefaultFlag;

	/* ��λ������ɱ�־ */
	g_MasterCompletionFlag = false;
	reVal = LPI2C_MasterTransferNonBlocking(GTP_I2C_MASTER, &g_m_handle, &masterXfer);
	if (reVal != kStatus_Success)
	{
		// handle hangs
		i2c_lockup_cnt++;
		if (i2c_lockup_cnt >= I2C_BUSY_LIMIT) {
			I2CLockupRecover();
		}
		return 1;
	}
	/* �ȴ�������� */
	while (!g_MasterCompletionFlag)
	{
    	if((i2c_timeout--) == 0)
			return I2C_Timeout_Callback(1);
	}
	g_MasterCompletionFlag = false;

	return 0;

}

/**
  * @brief  IIC�ȴ���ʱ���ñ��������������Ϣ
  * @param  None.
  * @retval ����0xff����ʾIIC��ȡ����ʧ��
  */
static  uint32_t I2C_Timeout_Callback(uint8_t errorCode)
{
	/* Block communication and all processes */
	// KPrintf("I2C timeout!errorCode = %d\n",errorCode);

	return 0xFF;
}

/**
* @brief  I2C���贫����ɵĻص�����
* @param  ��
* @retval ��
*/
static void I2C_Master_Callback(LPI2C_Type *base, lpi2c_master_handle_t *handle, status_t status, void *userData)
{		
    /* ���յ�kStatus_Success��־��
       ����g_MasterCompletionFlag��־��������ɹ� */
    if (status != kStatus_LPI2C_Nak) {
        g_MasterCompletionFlag = true;
        /* Display failure information when status is not success. */
        if (status != kStatus_Success)
        {
            // KPrintf("Error occured during transfer!.\n");
        }
    }
}



/**
* @brief  �������ż�оƬ��ʼ��
* @param  ��
* @retval ��
*/
void I2C_Touch_Init(void)
{

//  /* ��ʼ��I2C���蹤��ģʽ */
  GTP_I2C_ModeInit(); 
  
  /* ��λ����оƬ�����õ�ַ */
  GTP_ResetChip();
}


/***************************�ж����******************************/
/**
* @brief  ����оƬTOUCH_PAD_INT_GPIO_PIN���ŵ��жϷ�����
* @param  ��
* @retval ��
*/
//void TOUCH_PAD_IRQHANDLER(void)
extern int touch_sem;
void GT9xx_PEN_IRQHandler(int irqn, void *arg)
{
	DisableIRQ(GT9xx_PEN_IRQ);
    /* ȷ���Ǵ���оƬ���ж� */
    if(GPIO_GetPinsInterruptFlags(TOUCH_PAD_INT_GPIO) & 1U << TOUCH_PAD_INT_GPIO_PIN)
    {
       /* ����жϱ�־ */
      GPIO_PortClearInterruptFlags(TOUCH_PAD_INT_GPIO, 1U << TOUCH_PAD_INT_GPIO_PIN);
      /* �л���������״̬��־ */
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
int I2C_Transfer(struct i2c_msg* msgs, int num)
{
	int im = 0;
	int ret = 0;

	//GTP_DEBUG_FUNC();

	for (im = 0; ret == 0 && im != num; im++)
	{
		if (msgs[im].flags & I2C_M_RD)																//����flag�ж��Ƕ����ݻ���д����
		{
			ret = I2C_ReadBytes(msgs[im].addr, msgs[im].buf, msgs[im].len);		//IIC��ȡ����
		} else
		{
			ret = I2C_WriteBytes(msgs[im].addr, msgs[im].buf, msgs[im].len);	//IICд������
		}
	}

	if(ret)
		return ret;

	return im;   													//������ɵĴ���ṹ����
}












/*********************************************END OF FILE**********************/
