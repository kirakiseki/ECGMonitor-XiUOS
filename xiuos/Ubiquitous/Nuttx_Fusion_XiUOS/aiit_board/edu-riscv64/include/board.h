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
 * @file board.h
 * @brief edu-riscv64 board.h
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.03.17
 */

#ifndef __BOARDS_RISCV_K210_EDU_RISCV64_INCLUDE_BOARD_H
#define __BOARDS_RISCV_K210_EDU_RISCV64_INCLUDE_BOARD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
# include <stdint.h>
#endif

#include "k210.h"

#include "k210_fpioa.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Map pad 14 to gpiohs io 0 */
#define BOARD_LED_IO_FUNC K210_IO_FUNC_GPIOHS0

#define LED_STARTED       0  /* N/C */
#define LED_HEAPALLOCATE  1  /* N/C */
#define LED_IRQSENABLED   2  /* N/C */
#define LED_STACKCREATED  3  /* N/C */
#define LED_INIRQ         4  /* N/C */
#define LED_SIGNAL        5  /* N/C */
#define LED_ASSERTION     6  /* N/C */
#define LED_PANIC         7  /* blink */

/* GPIO pins used by the GPIO Subsystem */

#define BOARD_NGPIOOUT    2 /* Amount of register GPIO Output pins */
#define BOARD_NGPIOINT    0 /* Amount of GPIO Input */

/****************************************************************************
 * Public Types
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Public Data
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/*************************** GPIO define ***************************/

/* UART IO */
#define GPIO_WIFI_RXD      7
#define GPIO_WIFI_TXD      6
#define GPIO_E220_RXD     21
#define GPIO_E220_TXD     20
#define GPIO_CH376T_RXD   23
#define GPIO_CH376T_TXD   22

/* w5500 IO */
#define BSP_ENET_SCLK   9
#define BSP_ENET_MISO   10
#define BSP_ENET_MOSI   11
#define BSP_ENET_NCS    12
#define BSP_ENET_NRST   13
#define BSP_ENET_NINT   14

/* sdcard IO */
#define SOFT_SPI_MISO    25
#define SOFT_SPI_SCK     26
#define SOFT_SPI_MOSI    27
#define SOFT_SPI_CS      28


/* I2C  */
#define BSP_IIC_SDA     15
#define BSP_IIC_SCL     17

/* other mode io */
#define GPIO_E220_M0   32
#define GPIO_E220_M1   33
#define GPIO_WIFI_EN   8

/************************** end GPIO define **************************/


/*************************** FPIOA define ***************************/

/* UART FPOA */
#define FPOA_USART1_RX        K210_IO_FUNC_UART1_RX
#define FPOA_USART1_TX        K210_IO_FUNC_UART1_TX
#define FPOA_USART2_RX        K210_IO_FUNC_UART2_RX
#define FPOA_USART2_TX        K210_IO_FUNC_UART2_TX
#define FPOA_USART3_RX        K210_IO_FUNC_UART3_RX
#define FPOA_USART3_TX        K210_IO_FUNC_UART3_TX

/* w5500 FPIOA */
#define FPIOA_ENET_NRST 0
#define FPIOA_ENET_NINT 9
#define FPIOA_ENET_SCLK 28
#define FPIOA_ENET_MISO 29
#define FPIOA_ENET_MOSI 23
#define FPIOA_ENET_NCS  31

/* sdcard FPIOA */
#define FPIOA_SOFT_SPI_MISO    4
#define FPIOA_SOFT_SPI_SCK     5
#define FPIOA_SOFT_SPI_MOSI    6
#define FPIOA_SOFT_SPI_CS      7

/* other mode FPIOA */
#define FPIOA_E220_M0   1
#define FPIOA_E220_M1   2
#define FPIOA_WIFI_EN   3

/************************** end FPIOA define **************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: k210_boardinitialize
 ****************************************************************************/

void k210_boardinitialize(void);

#undef EXTERN
#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_RISC-V_K210_EDU_RISCV64_INCLUDE_BOARD_H */
