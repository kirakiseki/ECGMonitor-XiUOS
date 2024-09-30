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
 * @brief xidatong-riscv64 board.h
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.03.17
 */

#ifndef __BOARDS_RISCV_K210_XIDATONG_RISCV64_INCLUDE_BOARD_H
#define __BOARDS_RISCV_K210_XIDATONG_RISCV64_INCLUDE_BOARD_H

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

#define BOARD_NGPIOOUT    3 /* Amount of register GPIO Output pins */
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
#define GPIO_WIFI_RXD       7
#define GPIO_WIFI_TXD       6
#define GPIO_EC200T_RXD     21
#define GPIO_EC200T_TXD     20
#define GPIO_CH376T_RXD     22
#define GPIO_CH376T_TXD     23
#define GPIO_CAN_RXD        18
#define GPIO_CAN_TXD        19

/* ch438 IO */
#define CH438_ALE_PIN   24
#define CH438_NWR_PIN   25
#define CH438_NRD_PIN   26
#define CH438_D0_PIN    27
#define CH438_D1_PIN    28
#define CH438_D2_PIN    29
#define CH438_D3_PIN    30
#define CH438_D4_PIN    31
#define CH438_D5_PIN    32
#define CH438_D6_PIN    33
#define CH438_D7_PIN    34
#define CH438_INT_PIN   35

/* w5500 IO */
#define BSP_ENET_SCLK   9
#define BSP_ENET_MISO   10
#define BSP_ENET_MOSI   11
#define BSP_ENET_NCS    12
#define BSP_ENET_NRST   13
#define BSP_ENET_NINT   14

/* LCD IO */
#define BSP_LCD_NRST        37
#define BSP_LCD_SCLK        38
#define BSP_LCD_MOSI        39
#define BSP_LCD_MISO        40
#define BSP_LCD_NCS         41
#define BSP_LCD_BL_PIN      47

/* I2C  */
#define BSP_IIC_SDA     15
#define BSP_IIC_SCL     17

/* other mode io */
#define GPIO_E220_M0   44
#define GPIO_E220_M1   45
#define GPIO_E18_MODE  46
#define GPIO_WIFI_EN   8
#define GPIO_CAN_CFG   43

/************************** end GPIO define **************************/


/*************************** FPIOA define ***************************/

/* UART FPOA */
#define FPOA_USART1_RX        K210_IO_FUNC_UART1_RX
#define FPOA_USART1_TX        K210_IO_FUNC_UART1_TX
#define FPOA_USART2_RX        K210_IO_FUNC_UART2_RX
#define FPOA_USART2_TX        K210_IO_FUNC_UART2_TX
#define FPOA_USART3_RX        K210_IO_FUNC_UART3_RX
#define FPOA_USART3_TX        K210_IO_FUNC_UART3_TX

/* ch438 FPIOA */
#define FPIOA_CH438_ALE   11
#define FPIOA_CH438_NWR   12
#define FPIOA_CH438_NRD   13
#define FPIOA_CH438_D0    14
#define FPIOA_CH438_D1    15
#define FPIOA_CH438_D2    16
#define FPIOA_CH438_D3    17
#define FPIOA_CH438_D4    18
#define FPIOA_CH438_D5    29
#define FPIOA_CH438_D6    20
#define FPIOA_CH438_D7    31
#define FPIOA_CH438_INT   22

/* w5500 FPIOA */
#define FPIOA_ENET_NRST 0
#define FPIOA_ENET_NINT 9
#define FPIOA_ENET_SCLK 28
#define FPIOA_ENET_MISO 29
#define FPIOA_ENET_MOSI 23
#define FPIOA_ENET_NCS  31

/* LCD FPIOA */
#define FPIOA_LCD_NRST      0
#define FPIOA_LCD_BL        9
#define FPIOA_LCD_SCLK      28
#define FPIOA_LCD_MOSI      29
#define FPIOA_LCD_MISO      23 
#define FPIOA_LCD_NCS       31

/* I2C  */
#define FPIOA_IIC_SDA  7
#define FPIOA_IIC_SCL  8

/* other mode FPIOA */
#define FPIOA_E220_M0   1
#define FPIOA_E220_M1   2
#define FPIOA_E18_MODE  3
#define FPIOA_WIFI_EN   4
#define FPIOA_CAN_NCFG  5

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
#endif /* __BOARDS_RISC-V_K210_XIDATONG_RISCV64_INCLUDE_BOARD_H */
