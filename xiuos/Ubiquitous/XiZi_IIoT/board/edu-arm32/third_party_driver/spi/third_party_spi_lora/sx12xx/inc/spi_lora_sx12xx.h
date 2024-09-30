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
* @file spi_lora_sx12xx.h
* @brief define spi lora driver function
* @version 2.0 
* @author AIIT XUOS Lab
* @date 2022-10-31
*/

#ifndef SPI_LORA_SX12XX_H
#define SPI_LORA_SX12XX_H

#include <connect_spi_lora.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t SX1276ReadDio0(void);
uint8_t SX1276ReadDio1(void);
uint8_t SX1276ReadDio2(void);
uint8_t SX1276ReadDio3(void);
uint8_t SX1276ReadDio4(void);
uint8_t SX1276ReadDio5(void);

void SX1276Write(uint8_t addr, uint8_t data);
void SX1276Read(uint8_t addr, uint8_t *data);
void SX1276WriteBuffer(uint8_t addr, uint8_t *buffer, uint8_t size);
void SX1276ReadBuffer(uint8_t addr, uint8_t *buffer, uint8_t size);
void SX1276WriteFifo(uint8_t *buffer, uint8_t size);
void SX1276ReadFifo(uint8_t *buffer, uint8_t size);
void SX1276SetReset(uint8_t state);
uint8_t Sx1276SpiCheck(void);
void SX1276WriteRxTx(uint8_t txEnable);

#ifdef __cplusplus
}
#endif

#endif