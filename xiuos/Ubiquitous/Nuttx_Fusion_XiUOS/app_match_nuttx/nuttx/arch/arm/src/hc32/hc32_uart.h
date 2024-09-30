/****************************************************************************
 * arch/arm/src/hc32/hc32_uart.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __ARCH_ARM_STC_HC32_HC32_UART_H
#define __ARCH_ARM_STC_HC32_HC32_UART_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/serial/serial.h>
#include "chip.h"

#define CONSOLE_UART 6
#define HC32_NUSART 6
#ifndef USE_SERIALDRIVER
#define USE_SERIALDRIVER 1
#endif
#ifndef HAVE_SERIALDRIVER
#define HAVE_SERIALDRIVER 1
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

typedef enum en_uart_state
{
    UART_STATE_IDLE  = 0U,  /*!< No data */
    UART_STATE_RXEND = 1U,  /*!< UART RX End */
} uart_state_t;

/**
 * @brief  Ring buffer structure definition
 */
typedef struct
{
    uint16_t u16Capacity;
    volatile uint16_t u16UsedSize;
    uint16_t u16InIdx;
    uint16_t u16OutIdx;
    uint8_t  au8Buf[50];
} uart_ring_buffer_t;


/* UART multiple processor ID definition */
#define UART_MASTER_STATION_ID          (0x20U)
#define UART_SLAVE_STATION_ID           (0x21U)

/* Ring buffer size */
#define IS_RING_BUFFER_EMPTY(x)         (0U == ((x)->u16UsedSize))

/* Multi-processor silence mode */
#define DBG_UART_NORMAL_MODE          (0U)
#define DBG_UART_SILENCE_MODE         (1U)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifndef __ASSEMBLY__

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Functions Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: hc32_serial_get_uart
 *
 * Description:
 *   Get serial driver structure for HC32 UART
 *
 ****************************************************************************/

FAR uart_dev_t *hc32_serial_get_uart(int uart_num);

int hc32_print(const char *fmt, ...);

void hc32_console_handle(char *buf);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __ARCH_ARM_STC_HC32_HC32_UART_H */
