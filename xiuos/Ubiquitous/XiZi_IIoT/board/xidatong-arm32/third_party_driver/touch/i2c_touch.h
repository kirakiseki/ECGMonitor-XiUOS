#ifndef __I2C_TOUCH_H
#define	__I2C_TOUCH_H

#include "fsl_common.h"
#include "fsl_lpi2c.h"
#include "board.h"
#include <connect_touch.h>
extern lpi2c_master_handle_t g_m_handle;
extern volatile bool g_MasterCompletionFlag ;
extern volatile bool g_TouchPadInputSignal;
extern volatile bool SemReleaseFlag;



/* Select USB1 PLL (480 MHz) as master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_SELECT (0U)
/* Clock divider for master lpi2c clock source */
#define LPI2C_CLOCK_SOURCE_DIVIDER (5U)
/* Get frequency of lpi2c clock */
#define LPI2C_CLOCK_FREQUENCY ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 8) / (LPI2C_CLOCK_SOURCE_DIVIDER + 1U))

#define LPI2C_MASTER_CLOCK_FREQUENCY  LPI2C_CLOCK_FREQUENCY

#define GTP_I2C_MASTER_BASE   (LPI2C1_BASE)
#define GTP_I2C_MASTER        ((LPI2C_Type *)GTP_I2C_MASTER_BASE)

#define GTP_I2C_BAUDRATE 400000U

/* �ȴ���ʱʱ�� */
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x10000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

#define I2C_BUSY_LIMIT            5

/*! @brief ����оƬ���Ŷ��� */

#define TOUCH_PAD_SCL_IOMUXC			  IOMUXC_GPIO_AD_B1_00_LPI2C1_SCL
#define TOUCH_PAD_SDA_IOMUXC			  IOMUXC_GPIO_AD_B1_01_LPI2C1_SDA


#define TOUCH_PAD_RST_GPIO 			    GPIO1
#define TOUCH_PAD_RST_GPIO_PIN 	    (27U)
#define TOUCH_PAD_RST_IOMUXC			  IOMUXC_GPIO_AD_B1_11_GPIO1_IO27//IOMUXC_GPIO_AD_B1_11_GPIO1_IO27

#define TOUCH_PAD_INT_GPIO 			    GPIO2
#define TOUCH_PAD_INT_GPIO_PIN 	    (30U)
#define TOUCH_PAD_INT_IOMUXC			  IOMUXC_GPIO_AD_B0_11_GPIO1_IO11

#define GT9xx_PEN_IRQ           GPIO2_Combined_16_31_IRQn
#define GT9xx_PEN_IRQHandler    GPIO2_Combined_16_31_IRQHandler

#define TOUCH_PAD_RECOVER_SCL_GPIO          GPIO1
#define TOUCH_PAD_RECOVER_SCL_GPIO_PIN      (16U)
#define TOUCH_PAD_RECOVER_SDA_GPIO          GPIO1
#define TOUCH_PAD_RECOVER_SDA_GPIO_PIN      (17U)
#define TOUCH_PAD_RECOVER_SCL_IOMUXC    IOMUXC_GPIO_AD_B1_00_GPIO1_IO16
#define TOUCH_PAD_RECOVER_SDA_IOMUXC    IOMUXC_GPIO_AD_B1_01_GPIO1_IO17

#define I2C_RECOVER_NUM_CLOCKS      10U     /* # clock cycles for recovery  */
#define I2C_RECOVER_CLOCK_FREQ      50000U  /* clock frequency for recovery */


//�����ӿ�
int32_t GTP_I2C_ReadBytes(uint8_t client_addr, uint8_t *buf, int32_t len);

void I2C_Touch_Init(void);
void GtpI2cDeinit(void);
uint32_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite);
uint32_t I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead);
void GTP_ResetChip(void);
void GTP_IRQDisable(void);
void GTP_IRQEnable(void);
int I2C_Transfer( struct i2c_msg *msgs,int num);



#endif /* __I2C_TOUCH_H */
