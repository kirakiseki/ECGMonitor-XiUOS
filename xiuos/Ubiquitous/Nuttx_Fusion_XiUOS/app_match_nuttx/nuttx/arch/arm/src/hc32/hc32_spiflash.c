/**
 *******************************************************************************
 * @file  arch/arm/src/hc32/hc32_spiflash.c
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
#include "hc32_uart.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/* FLASH parameters */
#define FLASH_PAGE_SIZE                 (0x100U)
#define FLASH_SECTOR_SIZE               (0x1000U)
#define FLASH_MAX_ADDR                  (0x800000UL)
#define FLASH_DUMMY_BYTE_VALUE          (0xffU)
#define FLASH_BUSY_BIT_MASK             (0x01U)

/* FLASH instruction */
#define FLASH_INSTR_WRITE_ENABLE        (0x06U)
#define FLASH_INSTR_PAGE_PROGRAM        (0x02U)
#define FLASH_INSTR_STANDARD_READ       (0x03U)
#define FLASH_INSTR_ERASE_4KB_SECTOR    (0x20U)
#define FLASH_INSTR_READ_SR1            (0x05U)
#define FLASH_READ_MANUFACTURER_ID      (0x90U)

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
 * @brief SPI flash write byte function
 *
 * @param [in] u8Data                      SPI write data to flash
 *
 * @retval uint8_t                         SPI receive data from flash
 */
static uint8_t SpiFlash_WriteReadByte(uint8_t u8Data)
{
    uint8_t u8Byte;

    /* Wait tx buffer empty */
    while (Reset == SPI_GetStatus(SPI_UNIT, SPI_FLAG_TX_BUFFER_EMPTY))
    {
    }
    /* Send data */
    SPI_WriteDataReg(SPI_UNIT, (uint32_t)u8Data);

    /* Wait rx buffer full */
    while (Reset == SPI_GetStatus(SPI_UNIT, SPI_FLAG_RX_BUFFER_FULL))
    {
    }
    /* Receive data */
    u8Byte = (uint8_t)SPI_ReadDataReg(SPI_UNIT);

    return u8Byte;
}

/**
 * @brief SPI flash write enable function
 *
 * @param [in] None
 *
 * @retval None
 */
static void SpiFlash_WriteEnable(void)
{
    SPI_NSS_LOW();
    (void)SpiFlash_WriteReadByte(FLASH_INSTR_WRITE_ENABLE);
    SPI_NSS_HIGH();
}

/**
 * @brief SPI flash wait for write operation end function
 *
 * @param [in] None
 *
 * @retval Ok                              Flash internal operation finish
 * @retval ErrorTimeout                    Flash internal operation timeout
 */
static en_result_t SpiFlash_WaitForWriteEnd(void)
{
    en_result_t enRet = Ok;
    uint8_t u8Status;
    uint32_t u32Timeout;
    stc_clk_freq_t stcClkFreq;

    (void)CLK_GetClockFreq(&stcClkFreq);
    u32Timeout = stcClkFreq.sysclkFreq / 1000U;
    SPI_NSS_LOW();
    (void)SpiFlash_WriteReadByte(FLASH_INSTR_READ_SR1);
    do
    {
        u8Status = SpiFlash_WriteReadByte(FLASH_DUMMY_BYTE_VALUE);
        u32Timeout--;
    } while ((u32Timeout != 0UL) &&
             ((u8Status & FLASH_BUSY_BIT_MASK) == FLASH_BUSY_BIT_MASK));

    if (FLASH_BUSY_BIT_MASK == u8Status)
    {
        enRet = ErrorTimeout;
    }
    SPI_NSS_HIGH();

    return enRet;
}

/**
 * @brief SPI flash page write program function
 *
 * @param [in] u32Addr                     Valid flash address
 * @param [in] pData                       Pointer to send data buffer
 * @param [in] len                         Send data length
 *
 * @retval Error                           Page write program failed
 * @retval Ok                              Page write program success
 */
static en_result_t SpiFlash_WritePage(uint32_t u32Addr, const uint8_t pData[], uint16_t len)
{
    en_result_t enRet;
    uint16_t u16Index = 0U;

    if ((u32Addr > FLASH_MAX_ADDR) || (NULL == pData) || (len > FLASH_PAGE_SIZE))
    {
        enRet = Error;
    }
    else
    {
        SpiFlash_WriteEnable();
        /* Send data to flash */
        SPI_NSS_LOW();
        (void)SpiFlash_WriteReadByte(FLASH_INSTR_PAGE_PROGRAM);
        (void)SpiFlash_WriteReadByte((uint8_t)((u32Addr & 0xFF0000UL) >> 16U));
        (void)SpiFlash_WriteReadByte((uint8_t)((u32Addr & 0xFF00U) >> 8U));
        (void)SpiFlash_WriteReadByte((uint8_t)(u32Addr & 0xFFU));
        while (0U != (len--))
        {
            (void)SpiFlash_WriteReadByte(pData[u16Index]);
            u16Index++;
        }
        SPI_NSS_HIGH();
        /* Wait for flash idle */
        enRet = SpiFlash_WaitForWriteEnd();
    }

    return enRet;
}

/**
 * @brief SPI flash read data function
 *
 * @param [in] u32Addr                     Valid flash address
 * @param [out] pData                      Pointer to receive data buffer
 *
 * @param [in] len                         Read data length
 *
 * @retval Error                           Read data program failed
 * @retval Ok                              Read data program success
 */
static en_result_t SpiFlash_ReadData(uint32_t u32Addr, uint8_t pData[], uint16_t len)
{
    en_result_t enRet = Ok;
    uint16_t u16Index = 0U;

    if ((u32Addr > FLASH_MAX_ADDR) || (NULL == pData))
    {
        enRet = Error;
    }
    else
    {
        SpiFlash_WriteEnable();
        /* Send data to flash */
        SPI_NSS_LOW();
        (void)SpiFlash_WriteReadByte(FLASH_INSTR_STANDARD_READ);
        (void)SpiFlash_WriteReadByte((uint8_t)((u32Addr & 0xFF0000UL) >> 16U));
        (void)SpiFlash_WriteReadByte((uint8_t)((u32Addr & 0xFF00U) >> 8U));
        (void)SpiFlash_WriteReadByte((uint8_t)(u32Addr & 0xFFU));
        while (0U != (len--))
        {
            pData[u16Index] = SpiFlash_WriteReadByte(FLASH_DUMMY_BYTE_VALUE);
            u16Index++;
        }
        SPI_NSS_HIGH();
    }

    return enRet;
}

/**
 * @brief SPI flash read ID for test
 *
 * @param [in] None
 *
 * @retval uint8_t                         Flash ID
 */
static uint8_t SpiFlash_ReadID(void)
{
    uint8_t u8IdRead;
    SPI_NSS_LOW();
    (void)SpiFlash_WriteReadByte(FLASH_READ_MANUFACTURER_ID);
    (void)SpiFlash_WriteReadByte((uint8_t)0x00U);
    (void)SpiFlash_WriteReadByte((uint8_t)0x00U);
    (void)SpiFlash_WriteReadByte((uint8_t)0x00U);
    u8IdRead = SpiFlash_WriteReadByte(FLASH_DUMMY_BYTE_VALUE);
    SPI_NSS_HIGH();
    return u8IdRead;
}

/**
 * @brief SPI flash erase 4Kb sector function
 *
 * @param [in] u32Addr                     Valid flash address
 *
 * @retval Error                           Sector erase failed
 * @retval Ok                              Sector erase success
 */
static en_result_t SpiFlash_Erase4KbSector(uint32_t u32Addr)
{
    en_result_t enRet;

    if (u32Addr >= FLASH_MAX_ADDR)
    {
        enRet =  Error;
    }
    else
    {
        SpiFlash_WriteEnable();
        /* Send instruction to flash */
        SPI_NSS_LOW();
        (void)SpiFlash_WriteReadByte(FLASH_INSTR_ERASE_4KB_SECTOR);
        (void)SpiFlash_WriteReadByte((uint8_t)((u32Addr & 0xFF0000UL) >> 16U));
        (void)SpiFlash_WriteReadByte((uint8_t)((u32Addr & 0xFF00U) >> 8U));
        (void)SpiFlash_WriteReadByte((uint8_t)(u32Addr & 0xFFU));
        //SPI_GetStatus(const M4_SPI_TypeDef *SPIx, uint32_t u32Flag) //todo
        SPI_NSS_HIGH();
        /* Wait for flash idle */
        enRet = SpiFlash_WaitForWriteEnd();
    }

    return enRet;
}


/**
 * @brief  Main function of spi_master_base project
 * @param  None
 * @retval int32_t return value, if needed
 */

void hc32_spiflash_test(void)
{
    uint32_t flashAddr = 0UL;
    uint16_t bufferLen;
    char txBuffer[] = "SPI read and write flash example: Welcome to use HDSC micro chip";
    char rxBuffer[128];
    uint8_t flash_id = 0;

    hc32_spi_init();

    /* Get tx buffer length */
    bufferLen = (uint16_t)sizeof(txBuffer);

    flash_id = SpiFlash_ReadID();
    hc32_print("SPI Flash id: %#x\n", flash_id);

    (void)memset(rxBuffer, 0L, sizeof(rxBuffer));
    /* Erase sector */
    (void)SpiFlash_Erase4KbSector(flashAddr);
    /* Write data to flash */
    (void)SpiFlash_WritePage(flashAddr, (uint8_t*)&txBuffer[0], bufferLen);
    /* Read data from flash */
    (void)SpiFlash_ReadData(flashAddr, (uint8_t*)&rxBuffer[0], bufferLen);

    /* Compare txBuffer and rxBuffer */
    if (memcmp(txBuffer, rxBuffer, (uint32_t)bufferLen) != 0)
    {
        hc32_print("spi failed!!!\n");
    }
    else
    {
        hc32_print("spi ok!!!\n");
    }

    /* Flash address offset */
    flashAddr += FLASH_SECTOR_SIZE;
    if (flashAddr >= FLASH_MAX_ADDR)
    {
        flashAddr = 0U;
    }
}

int hc32_spidev_initialized = 1;

void hc32_spidev_initialize(void)
{
    hc32_spi_init();
    hc32_spidev_initialized = 0;
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

