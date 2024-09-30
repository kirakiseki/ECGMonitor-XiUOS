/**
 *******************************************************************************
 * @file  arch/arm/src/hc32/hc32_spi.c
 * @brief SPI write read flash API for the Device Driver Library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2020-06-12       Wangmin         First version
   2020-10-13       Wangmin         Modify spelling mistake
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
#include "hc32_ddl.h"
#include "hc32_spi.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/


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
    //SRAM_CKCR_Unlock();
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
    //SRAM_CKCR_Lock();
    /* Lock EFM OTP write protect registers */
    //EFM_OTP_WP_Lock();
    /* Lock EFM register: FWMC */
    //EFM_FWMC_Lock();
    /* Lock all EFM registers */
    EFM_Lock();
    /* Lock all MPU registers */
    // MPU_Lock();
}

/**
 * @brief Configure SPI peripheral function
 *
 * @param [in] None
 *
 * @retval None
 */
static void Spi_Config(void)
{
    stc_spi_init_t stcSpiInit;
    stc_spi_delay_t stcSpiDelayCfg;

    /* Clear initialize structure */
    (void)SPI_StructInit(&stcSpiInit);
    (void)SPI_DelayStructInit(&stcSpiDelayCfg);

    /* Configure peripheral clock */
    PWC_Fcg1PeriphClockCmd(SPI_UNIT_CLOCK, Enable);

    /* SPI De-initialize */
    SPI_DeInit(SPI_UNIT);

    /* Configuration SPI structure */
    stcSpiInit.u32WireMode          = SPI_WIRE_3;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = SPI_MASTER;
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;
    stcSpiInit.u32SpiMode           = SPI_MODE_0;
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_PCLK1_DIV256;
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;
    (void)SPI_Init(SPI_UNIT, &stcSpiInit);

    stcSpiDelayCfg.u32IntervalDelay = SPI_INTERVAL_TIME_8SCK_2PCLK1;
    stcSpiDelayCfg.u32ReleaseDelay = SPI_RELEASE_TIME_8SCK;
    stcSpiDelayCfg.u32SetupDelay = SPI_SETUP_TIME_1SCK;
    (void)SPI_DelayTimeCfg(SPI_UNIT, &stcSpiDelayCfg);

    SPI_FunctionCmd(SPI_UNIT, Enable);
}


int hc32_spi_init(void)
{
    stc_gpio_init_t stcGpioCfg;

    Peripheral_WE();

    /* Port configurate */
    (void)GPIO_StructInit(&stcGpioCfg);

    /* High driving capacity for output pin. */
    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    stcGpioCfg.u16PinState = PIN_STATE_SET;
    (void)GPIO_Init(SPI_NSS_PORT,  SPI_NSS_PIN, &stcGpioCfg);

    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(SPI_SCK_PORT,  SPI_SCK_PIN, &stcGpioCfg);
    (void)GPIO_Init(SPI_MOSI_PORT, SPI_MOSI_PIN, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinDrv = PIN_DRV_LOW;
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(SPI_MISO_PORT, SPI_MISO_PIN, &stcGpioCfg);

    /* Configure SPI Port function for master */
    GPIO_SetFunc(SPI_SCK_PORT,  SPI_SCK_PIN,  SPI_SCK_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(SPI_MOSI_PORT, SPI_MOSI_PIN, SPI_MOSI_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(SPI_MISO_PORT, SPI_MISO_PIN, SPI_MISO_FUNC, PIN_SUBFUNC_DISABLE);

    /* Configure SPI for SPI flash */
    Spi_Config();

    Peripheral_WP();
    return OK;
}

