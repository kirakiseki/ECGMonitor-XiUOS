/*
 * Copyright 2018-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
 
/**
* @file flash.h
* @brief support flexspi norflash function
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2023-04-03
*/

#ifndef __FLASH_H__
#define __FLASH_H__

#include <xs_base.h>
#include <stdint.h>
#include "fsl_romapi.h"

#define USE_HIGHT_SPEED_TRANS  1
#define SECTOR_SIZE 0x1000 
#define FLASH_PAGE_SIZE 256

#define FLEXSPI_WAIT_TIMEOUT_NS (500000000UL) //FlexSPI timeout value, 500ms
#define FLEXSPI_FREQ_1GHz (1000000000UL)
#define FREQ_1MHz (1000000UL)
#define FLEXSPI_DLLCR_DEFAULT (0x100UL)

enum
{
    kFlexSpiDelayCellUnit_Min = 75,  // 75ps
    kFlexSpiDelayCellUnit_Max = 225, // 225ps
};

typedef enum
{
    kFlexSpiClock_CoreClock,       //ARM Core Clock
    kFlexSpiClock_AhbClock,        //AHB clock
    kFlexSpiClock_SerialRootClock, //Serial Root Clock
    kFlexSpiClock_IpgClock,        //IPG clock
} flexspi_clock_type_t;

typedef struct LookUpTable
{
    uint32_t Read_Seq[4];
    uint32_t ReadStatus_Seq[8];
    uint32_t WriteEnable_Seq[8];
    uint32_t EraseSector_Seq[16];
    uint32_t PageProgram_Seq[8];
    uint32_t ChipErase_Seq[8];
    uint32_t ReadSfdp_Seq[4];
    uint32_t Restore_NoCmd_Seq[4];
    uint32_t Exit_NoCmd_Seq[4];
}lookuptable_t;


uint32_t FLASH_GetSectorSize(void);
uint32_t FLASH_GetProgramCmd(void);
void FLASH_Init(void);
void FLASH_DeInit(void);
uint8_t FLASH_EraseSector(uint32_t addr);
uint8_t FLASH_WritePage(uint32_t addr, const uint32_t *buf, uint32_t len);
status_t FLASH_Read(uint32_t addr, uint32_t *buf, uint32_t len);
status_t flash_erase(uint32_t start_addr, uint32_t byte_cnt);
status_t flash_write(uint32_t start_addr, uint8_t *buf, uint32_t byte_cnt);
status_t flash_read(uint32_t addr, uint8_t *buf, uint32_t len);
status_t flash_copy(uint32_t srcAddr,uint32_t dstAddr, uint32_t imageSize);

status_t NOR_FLASH_Erase(uint32_t app_base_addr,uint32_t imageSize);
void NorFlash_Write_PageProgram(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void NorFlash_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void NorFlash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);

#ifndef  USE_HIGHT_SPEED_TRANS
uint32_t NOR_FLASH_Write(uint32_t* FlashAddress, uint8_t* Data ,uint16_t DataLength);
#else
uint32_t NOR_FLASH_Write(uint32_t* FlashAddress, uint8_t* Data ,uint16_t DataLength,uint8_t doneFlag);
#endif

#endif

