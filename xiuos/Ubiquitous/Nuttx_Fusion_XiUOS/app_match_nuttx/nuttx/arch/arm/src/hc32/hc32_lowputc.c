/****************************************************************************
 * arch/arm/src/hc32/hc32_lowputc.c
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include <arch/board/board.h>

#include "arm_internal.h"
#include "chip.h"

#include "hc32_rcc.h"
#include "hc32_gpio.h"
#include "hc32_uart.h"
#include "hc32_lowputc.h"
#include "hc32f4a0_usart.h"
#include "hc32_ddl.h"
#include "hc32_uart.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

void hc32_unlock(void)
{
    /* Unlock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Unlock();
    /* Unlock PWC register: FCG0 */
    PWC_FCG0_Unlock();
    /* Unlock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Unlock(PWC_UNLOCK_CODE_0);
    /* Unlock SRAM register: WTCR */
    SRAM_WTCR_Unlock();
    /* Unlock SRAM register: CKCR */
    // SRAM_CKCR_Unlock();
    /* Unlock all EFM registers */
    EFM_Unlock();
}

void hc32_lock(void)
{
    /* Lock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Lock();
    /* Lock PWC register: FCG0 */
    PWC_FCG0_Lock();
    /* Lock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Lock(PWC_UNLOCK_CODE_0);
    /* Lock SRAM register: WTCR */
    SRAM_WTCR_Lock();
    /* Lock SRAM register: CKCR */
    // SRAM_CKCR_Lock();
    /* Lock EFM OTP write protect registers */
    // EFM_OTP_WP_Lock();
    /* Lock EFM register: FWMC */
    // EFM_FWMC_Lock();
    /* Lock all EFM registers */
    EFM_Lock();
}

void hc32_print_portinit(void)
{
    GPIO_SetFunc(BSP_PRINTF_PORT, BSP_PRINTF_PIN, BSP_PRINTF_PORT_FUNC, PIN_SUBFUNC_DISABLE);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: arm_lowputc
 *
 * Description:
 *   Output one byte on the serial console
 *
 ****************************************************************************/

void arm_lowputc(char ch)
{
    while(Set != USART_GetStatus(DBG_UNIT, USART_FLAG_TXE));
    USART_SendData(DBG_UNIT, ch);
}

void hc32_lowsetup(void)
{
    const stc_usart_uart_init_t stcUartInit = {
        .u32Baudrate = BSP_PRINTF_BAUDRATE,
        .u32BitDirection = USART_LSB,
        .u32StopBit = USART_STOPBIT_1BIT,
        .u32Parity = USART_PARITY_NONE,
        .u32DataWidth = USART_DATA_LENGTH_8BIT,
        .u32ClkMode = USART_INTERNCLK_OUTPUT,
        .u32PclkDiv = USART_PCLK_DIV64,
        .u32OversamplingBits = USART_OVERSAMPLING_8BIT,
        .u32NoiseFilterState = USART_NOISE_FILTER_DISABLE,
        .u32SbDetectPolarity = USART_SB_DETECT_FALLING,
    };

    hc32_unlock();

    hc32_clk_config();

    /* Initialize UART for debug print function. */
    DDL_PrintfInit(BSP_PRINTF_DEVICE, BSP_PRINTF_BAUDRATE, hc32_print_portinit);

    /* Configure USART RX/TX pin. */
    GPIO_SetFunc(DBG_RX_PORT, DBG_RX_PIN, DBG_RX_GPIO_FUNC, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(DBG_TX_PORT, DBG_TX_PIN, DBG_TX_GPIO_FUNC, PIN_SUBFUNC_DISABLE);

    hc32_lock();

    /* Enable peripheral clock */
    PWC_Fcg3PeriphClockCmd(DBG_FUNCTION_CLK_GATE, Enable);

    /* Initialize UART function. */
    if (Ok != USART_UartInit(DBG_UNIT, &stcUartInit))
    {
        for (;;);
    }
}

