/**
 *******************************************************************************
 * @file  i2c/i2c_master_polling/source/main.c
 * @brief Main program of I2C master polling for the Device Driver Library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2020-06-12       Hexiao         First version
   2020-08-31       Hexiao         Modify I2C init flow
   2020-09-04       Hexiao         Modify compile warning
   2021-01-21       Hexiao         Replace PWC_FCG1_IIC1 with PWC_FCG1_I2C1
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by HDSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <nuttx/config.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/arch.h>
#include <nuttx/irq.h>
#include <nuttx/clock.h>
#include <nuttx/semaphore.h>
#include <nuttx/i2c/i2c_master.h>
#include "hc32_ddl.h"
#include "hc32_gpio.h"
#include "hc32_uart.h"

/**
 * @addtogroup HC32F4A0_DDL_Examples
 * @{
 */

/**
 * @addtogroup I2C_Master_Polling
 * @{
 */

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* Define slave device address for example */
#define DEVICE_ADDRESS                  (0x77U) //bmp180
/* Define port and pin for SDA and SCL */
#define I2C_SCL_PORT                    (GPIO_PORT_D)
#define I2C_SCL_PIN                     (GPIO_PIN_03)
#define I2C_SDA_PORT                    (GPIO_PORT_F)
#define I2C_SDA_PIN                     (GPIO_PIN_10)

#define I2C_RET_OK                      (0U)
#define I2C_RET_ERROR                   (1U)

#define GENERATE_START                  (0x00U)
#define GENERATE_RESTART                (0x01U)

#define ADDRESS_W                       (0x00U)
#define ADDRESS_R                       (0x01U)

/* Define Write and read data length for the example */
#define TEST_DATA_LEN                   (1U)
/* Define i2c baudrate */
#define I2C_BAUDRATE                    (400000UL)

//#define LED_GREEN_PORT                  (GPIO_PORT_I)
//#define LED_GREEN_PIN                   (GPIO_PIN_03)

#define I2C_MASTER      1
#define I2C_SLAVE       2

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/* I2C Device hardware configuration */

struct hc32_i2c_config_s
{
  M4_I2C_TypeDef *base;              /* I2C base address */
  gpio_pinset_t scl_pin;      /* GPIO configuration for SCL as SCL */
  gpio_pinset_t sda_pin;      /* GPIO configuration for SDA as SDA */
  uint8_t mode;               /* Master or Slave mode */
#ifndef CONFIG_I2C_POLLED
  uint32_t irq;               /* Event IRQ */
#endif
};

/* I2C Device Private Data */

struct hc32_i2c_priv_s
{
  /* Standard I2C operations */

  const struct i2c_ops_s *ops;

  /* Port configuration */

  const struct hc32_i2c_config_s *config;

  int refs;                    /* Reference count */
  sem_t sem_excl;              /* Mutual exclusion semaphore */
#ifndef CONFIG_I2C_POLLED
  sem_t sem_isr;               /* Interrupt wait semaphore */
#endif
  volatile uint8_t intstate;   /* Interrupt handshake (see enum hc32_intstate_e) */

  uint8_t msgc;                /* Message count */
  struct i2c_msg_s *msgv;      /* Message list */
  uint8_t *ptr;                /* Current message buffer */
  uint32_t frequency;          /* Current I2C frequency */
  int dcnt;                    /* Current message length */
  uint16_t flags;              /* Current message flags */

  uint32_t status;             /* End of transfer SR2|SR1 status */
};

static int hc32_i2c_init(FAR struct hc32_i2c_priv_s *priv);
static int hc32_i2c_deinit(FAR struct hc32_i2c_priv_s *priv);
static int hc32_i2c_transfer(FAR struct i2c_master_s *dev,
                               FAR struct i2c_msg_s *msgs, int count);
#ifdef CONFIG_I2C_RESET
static int hc32_i2c_reset(FAR struct i2c_master_s *dev);
#endif

/* I2C interface */

static const struct i2c_ops_s hc32_i2c_ops =
{
  .transfer = hc32_i2c_transfer
#ifdef CONFIG_I2C_RESET
  , .reset  = hc32_i2c_reset
#endif
};

/* I2C device structures */

#ifdef CONFIG_HC32_I2C1
static const struct hc32_i2c_config_s hc32_i2c1_config =
{
  .base       = M4_I2C1,
  .scl_pin    = GPIO_PINSET(I2C_SCL_PORT, I2C_SCL_PIN),
  .sda_pin    = GPIO_PINSET(I2C_SDA_PORT, I2C_SDA_PIN),
#ifndef CONFIG_I2C_SLAVE
  .mode       = I2C_MASTER,
#else
  .mode       = I2C_SLAVE,
#endif
#ifndef CONFIG_I2C_POLLED
  .irq        = EVT_I2C1_RXI,
#endif
};

static struct hc32_i2c_priv_s hc32_i2c1_priv =
{
  .ops        = &hc32_i2c_ops,
  .config     = &hc32_i2c1_config,
  .refs       = 0,
  .intstate   = INTSTATE_IDLE,
  .msgc       = 0,
  .msgv       = NULL,
  .ptr        = NULL,
  .dcnt       = 0,
  .flags      = 0,
  .status     = 0
};
#endif

#ifdef CONFIG_HC32_I2C2
static const struct hc32_i2c_config_s hc32_i2c2_config =
{
  .base       = M4_I2C2,
  .scl_pin    = GPIO_PINSET(I2C_SCL_PORT, I2C_SCL_PIN),
  .sda_pin    = GPIO_PINSET(I2C_SDA_PORT, I2C_SDA_PIN),
#ifndef CONFIG_I2C_SLAVE
  .mode       = I2C_MASTER,
#else
  .mode       = I2C_SLAVE,
#endif
#ifndef CONFIG_I2C_POLLED
  .irq        = EVT_I2C2_RXI,
#endif
};

static struct hc32_i2c_priv_s hc32_i2c2_priv =
{
  .ops        = &hc32_i2c_ops,
  .config     = &hc32_i2c2_config,
  .refs       = 0,
  .intstate   = INTSTATE_IDLE,
  .msgc       = 0,
  .msgv       = NULL,
  .ptr        = NULL,
  .dcnt       = 0,
  .flags      = 0,
  .status     = 0
};
#endif

#ifdef CONFIG_HC32_I2C3
static const struct hc32_i2c_config_s hc32_i2c3_config =
{
  .base       = M4_I2C3,
  .scl_pin    = GPIO_PINSET(I2C_SCL_PORT, I2C_SCL_PIN),
  .sda_pin    = GPIO_PINSET(I2C_SDA_PORT, I2C_SDA_PIN),
#ifndef CONFIG_I2C_SLAVE
  .mode       = I2C_MASTER,
#else
  .mode       = I2C_SLAVE,
#endif
#ifndef CONFIG_I2C_POLLED
  .irq        = EVT_I2C3_RXI,
#endif
};

static struct hc32_i2c_priv_s hc32_i2c3_priv =
{
  .ops        = &hc32_i2c_ops,
  .config     = &hc32_i2c3_config,
  .refs       = 0,
  .intstate   = INTSTATE_IDLE,
  .msgc       = 0,
  .msgv       = NULL,
  .ptr        = NULL,
  .dcnt       = 0,
  .flags      = 0,
  .status     = 0
};
#endif

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
#define I2C_TIMEOUT                     (0x24000U)

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 * @brief  MCU Peripheral registers write unprotected.
 * @param  None
 * @retval None
 * @note Comment/uncomment each API depending on APP requires.
 */
static void Peripheral_WE(void)
{
    /* Unlock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Unlock();
    /* Unlock PWC register: FCG0 */
    PWC_FCG0_Unlock();
    /* Unlock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Unlock(PWC_UNLOCK_CODE_0 | PWC_UNLOCK_CODE_1 | PWC_UNLOCK_CODE_2);
    /* Unlock SRAM register: WTCR */
    SRAM_WTCR_Unlock();
    /* Unlock SRAM register: CKCR */
    SRAM_CKCR_Unlock();
    /* Unlock all EFM registers */
    EFM_Unlock();
    /* Unlock EFM register: FWMC */
    //EFM_FWMC_Unlock();
    /* Unlock EFM OTP write protect registers */
    //EFM_OTP_WP_Unlock();
    /* Unlock all MPU registers */
    // MPU_Unlock();
}

/**
 * @brief  MCU Peripheral registers write protected.
 * @param  None
 * @retval None
 * @note Comment/uncomment each API depending on APP requires.
 */
static __attribute__((unused)) void Peripheral_WP(void)
{
    /* Lock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Lock();
    /* Lock PWC register: FCG0 */
    PWC_FCG0_Lock();
    /* Lock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Lock(PWC_UNLOCK_CODE_0 | PWC_UNLOCK_CODE_1 | PWC_UNLOCK_CODE_2);
    /* Lock SRAM register: WTCR */
    SRAM_WTCR_Lock();
    /* Lock SRAM register: CKCR */
    SRAM_CKCR_Lock();
    /* Lock EFM OTP write protect registers */
    //EFM_OTP_WP_Lock();
    /* Lock EFM register: FWMC */
    //EFM_FWMC_Lock();
    /* Lock all EFM registers */
    EFM_Lock();
    /* Lock all MPU registers */
    // MPU_Lock();
}

//static void Master_LedInit(void)
//{
//    stc_gpio_init_t stcGpioInit;
//
//    /* RGB LED initialize */
//    (void)GPIO_StructInit(&stcGpioInit);
//    (void)GPIO_Init(LED_GREEN_PORT, LED_GREEN_PIN, &stcGpioInit);
//
//    /* "Turn off" LED before set to output */
//    GPIO_ResetPins(LED_GREEN_PORT, LED_GREEN_PIN);
//
//    /* Output enable */
//    GPIO_OE(LED_GREEN_PORT, LED_GREEN_PIN, Enable);
//}

//static void Master_LedOn(void)
//{
//    GPIO_SetPins(LED_GREEN_PORT, LED_GREEN_PIN);
//}

/**
 * @brief   Send start or restart condition
 * @param   [in]  u8Start  Indicate the start mode, start or restart
 * @retval  Process result
 *          - I2C_RET_ERROR  Send start or restart failed
 *          - I2C_RET_OK     Send start or restart success
 */
static en_result_t Master_StartOrRestart(uint8_t u8Start)
{
    en_result_t enRet;

    /* generate start or restart signal */
    if(GENERATE_START == u8Start)
    {
        enRet = I2C_Start(M4_I2C1,I2C_TIMEOUT);
    }
    else
    {
        /* Clear start status flag */
        enRet = I2C_Restart(M4_I2C1,I2C_TIMEOUT);
    }

    return enRet;
}

/**
 * @brief   Send slave address
 * @param   [in] u8Adr  The slave address
 * @param   [in] u8Dir The transfer direction @ref I2C_Transfer_Direction
 * @retval  Process result
 *          - I2C_RET_ERROR  Send failed
 *          - I2C_RET_OK     Send success
 */
static en_result_t Master_SendAdr(uint8_t u8Adr, uint8_t u8Dir)
{
   return I2C_TransAddr(M4_I2C1,u8Adr,u8Dir,I2C_TIMEOUT);
}

/**
 * @brief   Send data to slave
 * @param   [in]  pTxData  Pointer to the data buffer
 * @param   [in]  u32Size  Data size
 * @retval  Process result
 *          - I2C_RET_ERROR  Send failed
 *          - I2C_RET_OK     Send success
 */
static en_result_t Master_WriteData(uint8_t const pTxData[], uint32_t u32Size)
{
    return I2C_TransData(M4_I2C1, pTxData, u32Size,I2C_TIMEOUT);
}

/**
 * @brief   Write address and receive data from slave
 * @param   [in]  pRxData  Pointer to the data buffer
 * @param   [in]  u32Size  Data size
 * @retval  Process result
 *          - I2C_RET_ERROR  Process failed
 *          - I2C_RET_OK     Process success
 */
static en_result_t Master_ReceiveAndStop(uint8_t pRxData[], uint32_t u32Size)
{
    return I2C_MasterReceiveAndStop(M4_I2C1,pRxData, u32Size,I2C_TIMEOUT);
}

/**
 * @brief   General stop condition to slave
 * @param   None
 * @retval  Process result
 *          - I2C_RET_ERROR  Process failed
 *          - I2C_RET_OK     Process success
 */
static en_result_t Master_Stop(void)
{
   return I2C_Stop(M4_I2C1,I2C_TIMEOUT);
}

/**
 * @brief   Initialize the I2C peripheral for master
 * @param   None
 * @retval  Process result
 *          - I2C_RET_ERROR  Process failed
 *          - I2C_RET_OK     Process success
 */
static en_result_t Master_Initialize(void)
{
    stc_i2c_init_t stcI2cInit;
    float32_t fErr;

    I2C_DeInit(M4_I2C1);

    (void)I2C_StructInit(&stcI2cInit);
    stcI2cInit.u32Baudrate = I2C_BAUDRATE;
    stcI2cInit.u32SclTime = 5U;
    stcI2cInit.u32ClkDiv = I2C_CLK_DIV4;
    en_result_t enRet = I2C_Init(M4_I2C1, &stcI2cInit, &fErr);
    I2C_BusWaitCmd(M4_I2C1, Enable);

    if(enRet == Ok)
    {
        I2C_Cmd(M4_I2C1, Enable);
    }

    return enRet;
}

/**
 * @brief   Judge the result. LED0 toggle when result is error status.
 * @param   [in]  enRet    Result to be judged
 * @retval  None
 */
static void JudgeResult(en_result_t enRet)
{
    if(Ok != enRet)
    {
        for(;;)
        {
            DDL_DelayMS(500U);
        }
    }
}

static int hc32_i2c_init(FAR struct hc32_i2c_priv_s *priv)
{
    return OK;
}

static int hc32_i2c_deinit(FAR struct hc32_i2c_priv_s *priv)
{
    return OK;
}

static inline void hc32_i2c_sem_init(FAR struct hc32_i2c_priv_s *priv)
{
  nxsem_init(&priv->sem_excl, 0, 1);

#ifndef CONFIG_I2C_POLLED
  /* This semaphore is used for signaling and, hence, should not have
   * priority inheritance enabled.
   */

  nxsem_init(&priv->sem_isr, 0, 0);
  nxsem_set_protocol(&priv->sem_isr, SEM_PRIO_NONE);
#endif
}


static inline void hc32_i2c_sem_destroy(FAR struct hc32_i2c_priv_s *priv)
{
  nxsem_destroy(&priv->sem_excl);
#ifndef CONFIG_I2C_POLLED
  nxsem_destroy(&priv->sem_isr);
#endif
}

/****************************************************************************
 * Name: hc32_i2c_sem_post
 *
 * Description:
 *   Release the mutual exclusion semaphore
 *
 ****************************************************************************/

static inline void hc32_i2c_sem_post(struct hc32_i2c_priv_s *priv)
{
  nxsem_post(&priv->sem_excl);
}

#if 0
static int hc32_i2c_transfer1(FAR struct i2c_master_s *dev,
                               FAR struct i2c_msg_s *msgs, int count)
{
  FAR struct hc32_i2c_priv_s *priv = (struct hc32_i2c_priv_s *)dev;
  uint32_t status = 0;
#ifdef I2C1_FSMC_CONFLICT
  uint32_t ahbenr;
#endif
  int ret;

  DEBUGASSERT(count);

  /* Ensure that address or flags don't change meanwhile */

  ret = nxsem_wait(&priv->sem_excl);
  if (ret < 0)
    {
      return ret;
    }
//
//

  /* Ensure that any ISR happening after we finish can't overwrite any user
   * data
   */

  priv->dcnt = 0;
  priv->ptr = NULL;

  hc32_i2c_sem_post(priv);
  return ret;
}
#endif

static int hc32_i2c_transfer(FAR struct i2c_master_s *dev,
                               FAR struct i2c_msg_s *msgs, int count)
{
    en_result_t ret;
    FAR struct hc32_i2c_priv_s *priv = (struct hc32_i2c_priv_s *)dev;

    /* Initialize I2C peripheral and enable function*/
    ret = Master_Initialize();
    JudgeResult(ret);

    /* I2C master data write*/
    ret = Master_StartOrRestart(GENERATE_START);
    JudgeResult(ret);
    ret = Master_SendAdr(msgs->addr, I2C_DIR_TX);
    JudgeResult(ret);
    ret = Master_WriteData(msgs->buffer, msgs->length);
    JudgeResult(ret);
    ret = Master_Stop();
    JudgeResult(ret);

    /* 5mS delay for device*/
    DDL_DelayMS(5U);

    /* I2C master data read*/
    ret = Master_StartOrRestart(GENERATE_START);
    if(1UL == msgs->length)
    {
        I2C_AckConfig(priv->config->base, I2C_NACK);
    }
    JudgeResult(ret);
    ret = Master_SendAdr(msgs->addr, I2C_DIR_RX);
    JudgeResult(ret);
    ret = Master_ReceiveAndStop(msgs->buffer, msgs->length);
    JudgeResult(ret);

    return ret;
}

int hc32_i2c_config(void)
{
    /* Initialize I2C port*/
    stc_gpio_init_t stcGpioInit;

    Peripheral_WE();

    (void)GPIO_StructInit(&stcGpioInit);
    (void)GPIO_Init(I2C_SCL_PORT, I2C_SCL_PIN, &stcGpioInit);
    (void)GPIO_Init(I2C_SDA_PORT, I2C_SDA_PIN, &stcGpioInit);
    GPIO_SetFunc(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_FUNC_49_I2C1_SCL, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_FUNC_48_I2C1_SDA, PIN_SUBFUNC_DISABLE);

    /* Enable peripheral clock */
    PWC_Fcg1PeriphClockCmd(PWC_FCG1_I2C1, Enable);
    return OK;
}


/**
 * @brief  BSP clock initialize.
 *         Set board system clock to PLLH@240MHz
 *         Flash: 5 wait
 *         SRAM_HS: 1 wait
 *         SRAM1_2_3_4_B: 2 wait
 *         PCLK0: 240MHz
 *         PCLK1: 120MHz
 *         PCLK2: 60MHz
 *         PCLK3: 60MHz
 *         PCLK4: 120MHz
 *         EXCLK: 120MHz
 *         HCLK:  240MHz
 * @param  None
 * @retval None
 */
void BSP_CLK_Init(void)
{
    stc_clk_xtal_init_t stcXtalInit;
    stc_clk_pllh_init_t stcPLLHInit;

    /* PCLK0, HCLK  Max 240MHz */
    /* PCLK1, PCLK4 Max 120MHz */
    /* PCLK2, PCLK3 Max 60MHz  */
    /* EX BUS Max 120MHz */
    CLK_ClkDiv(CLK_CATE_ALL,                                                   \
               (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 |             \
                CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |             \
                CLK_HCLK_DIV1));

    CLK_XtalStructInit(&stcXtalInit);
    /* Config Xtal and enable Xtal */
    stcXtalInit.u8XtalMode = CLK_XTALMODE_OSC;
    stcXtalInit.u8XtalDrv = CLK_XTALDRV_LOW;
    stcXtalInit.u8XtalState = CLK_XTAL_ON;
    stcXtalInit.u8XtalStb = CLK_XTALSTB_2MS;
    CLK_XtalInit(&stcXtalInit);

    (void)CLK_PLLHStructInit(&stcPLLHInit);
    /* VCO = (8/1)*120 = 960MHz*/
    stcPLLHInit.u8PLLState = CLK_PLLH_ON;
    stcPLLHInit.PLLCFGR = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM = 1UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLN = 120UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLP = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLQ = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLR = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLLSRC_XTAL;
    (void)CLK_PLLHInit(&stcPLLHInit);

    /* Highspeed SRAM set to 1 Read/Write wait cycle */
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE_1, SRAM_WAIT_CYCLE_1);

    /* SRAM1_2_3_4_backup set to 2 Read/Write wait cycle */
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE_2, SRAM_WAIT_CYCLE_2);

    /* 0-wait @ 40MHz */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE_5);

    /* 4 cycles for 200 ~ 250MHz */
    GPIO_SetReadWaitCycle(GPIO_READ_WAIT_4);

    CLK_SetSysClkSrc(CLK_SYSCLKSOURCE_PLLH);
}


/**
 * @brief  Main function of i2c_master_polling project
 * @param  None
 * @retval int32_t return value, if needed
 */
int hc32_i2c_test(void)
{
    en_result_t enRet;
    uint8_t u8TxBuf[TEST_DATA_LEN] = {0xD0};
    uint8_t u8RxBuf[TEST_DATA_LEN] = {0U};

    hc32_i2c_config();

    hc32_print("%s: %d start ...\n", __func__, __LINE__);

    /* Initialize I2C peripheral and enable function*/
    enRet = Master_Initialize();
    JudgeResult(enRet);

    hc32_print("%s: %d ret %d\n", __func__, __LINE__, enRet);

    /* I2C master data write*/
    enRet = Master_StartOrRestart(GENERATE_START);
    JudgeResult(enRet);
    enRet = Master_SendAdr(DEVICE_ADDRESS, I2C_DIR_TX);
    JudgeResult(enRet);
    enRet = Master_WriteData(u8TxBuf, TEST_DATA_LEN);
    JudgeResult(enRet);
    enRet = Master_Stop();
    JudgeResult(enRet);

    /* 5mS delay for device*/
    DDL_DelayMS(5U);

    /* I2C master data read*/
    enRet = Master_StartOrRestart(GENERATE_START);
    if(1UL == TEST_DATA_LEN)
    {
        I2C_AckConfig(M4_I2C1, I2C_NACK);
    }

    JudgeResult(enRet);
    enRet = Master_SendAdr(DEVICE_ADDRESS,I2C_DIR_RX);
    JudgeResult(enRet);
    enRet = Master_ReceiveAndStop(u8RxBuf, TEST_DATA_LEN);
    JudgeResult(enRet);

    hc32_print("%s: i2c device id = %x\n", __func__, u8RxBuf[0]);
    return OK;
}


/****************************************************************************
 * Name: hc32_i2cbus_initialize
 *
 * Description:
 *   Initialize one I2C bus
 *
 ****************************************************************************/

FAR struct i2c_master_s *hc32_i2cbus_initialize(int port)
{
  struct hc32_i2c_priv_s * priv = NULL;
  irqstate_t flags;

  /* Get I2C private structure */

  switch (port)
    {
#ifdef CONFIG_HC32_I2C1
    case 0:
      priv = (struct hc32_i2c_priv_s *)&hc32_i2c1_priv;
      break;
#endif
#ifdef CONFIG_HC32_I2C2
    case 1:
      priv = (struct hc32_i2c_priv_s *)&hc32_i2c2_priv;
      break;
#endif
#ifdef CONFIG_HC32_I2C3
    case 2:
      priv = (struct hc32_i2c_priv_s *)&hc32_i2c3_priv;
      break;
#endif
    default:
      return NULL;
    }

  /* Initialize private data for the first time, increment reference count,
   * power-up hardware and configure GPIOs.
   */

  flags = enter_critical_section();

  if ((volatile int)priv->refs++ == 0)
    {
      hc32_i2c_sem_init(priv);
      hc32_i2c_init(priv);
    }

  leave_critical_section(flags);

  return (struct i2c_master_s *)priv;
}

/****************************************************************************
 * Name: hc32_i2cbus_uninitialize
 *
 * Description:
 *   Uninitialize an I2C bus
 *
 ****************************************************************************/

int hc32_i2cbus_uninitialize(FAR struct i2c_master_s *dev)
{
  FAR struct hc32_i2c_priv_s *priv = (struct hc32_i2c_priv_s *)dev;
  irqstate_t flags;

  DEBUGASSERT(dev);

  /* Decrement reference count and check for underflow */

  if (priv->refs == 0)
    {
      return ERROR;
    }

  flags = enter_critical_section();

  if (--priv->refs > 0)
    {
      leave_critical_section(flags);
      return OK;
    }

  leave_critical_section(flags);

  /* Disable power and other HW resource (GPIO's) */

  hc32_i2c_deinit(priv);

  /* Release unused resources */

  hc32_i2c_sem_destroy(priv);
  return OK;
}


static void hc32_i2c_register(int bus)
{
  FAR struct i2c_master_s *i2c;
  int ret;

  i2c = hc32_i2cbus_initialize(bus);
  if (i2c == NULL)
    {
      syslog(LOG_ERR, "Failed to get I2C%d interface\n", bus);
    }
  else
    {
      ret = i2c_register(i2c, bus);
      if (ret < 0)
        {
          syslog(LOG_ERR, "Failed to register I2C%d driver: %d\n", bus, ret);
          hc32_i2cbus_uninitialize(i2c);
        }
    }
}

/**
 * @}
 */

/**
 * @}
 */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
