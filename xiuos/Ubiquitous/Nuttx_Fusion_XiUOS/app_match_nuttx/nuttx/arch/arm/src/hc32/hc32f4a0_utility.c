/**
 *******************************************************************************
 * @file  hc32f4a0_utility.c
 * @brief This file provides utility functions for DDL.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2020-06-12       Yangjp          First version
   2020-12-03       Yangjp          Fixed SysTick_Delay function overflow handling
   2021-04-15       Hongjh          Re-implement print initialization API: UART_PrintfInit.
   2021-05-24       Hongjh          Delete code for writing LBMC register in UART_PrintfInit.
   2021-08-19       Yangjp          Delete error compensation in SysTick_Delay function.
   2021-10-08       Hongjh          1. Rename function parameter for UART_PrintfInit.
                                    2. Added AC6 supporting of printf retartget.
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
#include "hc32f4a0_utility.h"

/**
 * @addtogroup HC32F4A0_DDL_Driver
 * @{
 */

/**
 * @defgroup DDL_UTILITY UTILITY
 * @brief DDL Utility Driver
 * @{
 */

#if (DDL_UTILITY_ENABLE == DDL_ON)

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/**
 * @defgroup UTILITY_Local_Macros UTILITY Local Macros
 * @{
 */

#if (DDL_PRINT_ENABLE == DDL_ON)
/*!< Parameter valid check for USART Instances. */
#define IS_USART_UNIT(x)                                                       \
(   ((x) == M4_USART1)                          ||                             \
    ((x) == M4_USART2)                          ||                             \
    ((x) == M4_USART3)                          ||                             \
    ((x) == M4_USART4)                          ||                             \
    ((x) == M4_USART5)                          ||                             \
    ((x) == M4_USART6)                          ||                             \
    ((x) == M4_USART7)                          ||                             \
    ((x) == M4_USART8)                          ||                             \
    ((x) == M4_USART9)                          ||                             \
    ((x) == M4_USART10))

#define UART_EnableClk(x)                                                      \
do {                                                                           \
    if ((x) == M4_USART1)                                                      \
    {                                                                          \
        CLEAR_REG32_BIT(M4_PWC->FCG3, PWC_FCG3_USART1);                        \
    }                                                                          \
    else if ((x) == M4_USART2)                                                 \
    {                                                                          \
        CLEAR_REG32_BIT(M4_PWC->FCG3, PWC_FCG3_USART2);                        \
    }                                                                          \
    else if ((x) == M4_USART3)                                                 \
    {                                                                          \
        CLEAR_REG32_BIT(M4_PWC->FCG3, PWC_FCG3_USART3);                        \
    }                                                                          \
    else if ((x) == M4_USART4)                                                 \
    {                                                                          \
        CLEAR_REG32_BIT(M4_PWC->FCG3, PWC_FCG3_USART4);                        \
    }                                                                          \
    else if ((x) == M4_USART5)                                                 \
    {                                                                          \
        CLEAR_REG32_BIT(M4_PWC->FCG3, PWC_FCG3_USART5);                        \
    }                                                                          \
    else if ((x) == M4_USART6)                                                 \
    {                                                                          \
        CLEAR_REG32_BIT(M4_PWC->FCG3, PWC_FCG3_USART2);                        \
    }                                                                          \
    else if ((x) == M4_USART7)                                                 \
    {                                                                          \
        CLEAR_REG32_BIT(M4_PWC->FCG3, PWC_FCG3_USART3);                        \
    }                                                                          \
    else if ((x) == M4_USART8)                                                 \
    {                                                                          \
        CLEAR_REG32_BIT(M4_PWC->FCG3, PWC_FCG3_USART4);                        \
    }                                                                          \
    else if ((x) == M4_USART9)                                                 \
    {                                                                          \
        CLEAR_REG32_BIT(M4_PWC->FCG3, PWC_FCG3_USART5);                        \
    }                                                                          \
    else                                                                       \
    {                                                                          \
        CLEAR_REG32_BIT(M4_PWC->FCG3, PWC_FCG3_USART6);                        \
    }                                                                          \
} while (0)
#endif

/**
 * @}
 */

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/**
 * @addtogroup UTILITY_Local_Functions UTILITY Local Functions
 * @{
 */

#if (DDL_PRINT_ENABLE == DDL_ON)

static en_result_t UartPutChar(M4_USART_TypeDef *USARTx, char cData);
static en_result_t UartSetBaudrate(M4_USART_TypeDef *USARTx,
                                    uint32_t u32Baudrate);
#endif

/**
 * @}
 */

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
/**
 * @defgroup UTILITY_Local_Variables UTILITY Local Variables
 * @{
 */

static uint32_t m_u32TickStep = 0UL;
static __IO uint32_t m_u32TickCount = 0UL;

#if (DDL_PRINT_ENABLE == DDL_ON)
static M4_USART_TypeDef *m_PrintfDevice;
static uint32_t m_u32PrintfTimeout;
#endif

/**
 * @}
 */

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @defgroup UTILITY_Global_Functions UTILITY Global Functions
 * @{
 */

/**
 * @brief Delay function, delay ms approximately
 * @param [in] u32Cnt                   ms
 * @retval None
 */
void DDL_DelayMS(uint32_t u32Cnt)
{
    __IO uint32_t i;
    const uint32_t u32Cyc = HCLK_VALUE / 10000UL;

    while (u32Cnt-- > 0UL)
    {
        i = u32Cyc;
        while (i-- > 0UL)
        {
            ;
        }
    }
}

/**
 * @brief Delay function, delay us approximately
 * @param [in] u32Cnt                   us
 * @retval None
 */
void DDL_DelayUS(uint32_t u32Cnt)
{
    __IO uint32_t i;
    const uint32_t u32Cyc = HCLK_VALUE / 10000000UL;

    while (u32Cnt-- > 0UL)
    {
        i = u32Cyc;
        while (i-- > 0UL)
        {
            ;
        }
    }
}

/**
 * @brief This function Initializes the interrupt frequency of the SysTick.
 * @param [in] u32Freq                  SysTick interrupt frequency (1 to 1000).
 * @retval An en_result_t enumeration value:
 *           - Ok: SysTick Initializes succeed
 *           - Error: SysTick Initializes failed
 */
__WEAKDEF en_result_t SysTick_Init(uint32_t u32Freq)
{
    en_result_t enRet = Error;

    if ((0UL != u32Freq) && (u32Freq <= 1000UL))
    {
        m_u32TickStep = 1000UL / u32Freq;
        /* Configure the SysTick interrupt */
        if (0UL == SysTick_Config(HCLK_VALUE / u32Freq))
        {
            enRet = Ok;
        }
    }

    return enRet;
}

/**
 * @brief This function provides minimum delay (in milliseconds).
 * @param [in] u32Delay                 Delay specifies the delay time.
 * @retval None
 */
__WEAKDEF void SysTick_Delay(uint32_t u32Delay)
{
    const uint32_t tickStart = SysTick_GetTick();
    uint32_t tickEnd = u32Delay;
    uint32_t tickMax;

    if (m_u32TickStep != 0UL)
    {
        tickMax = 0xFFFFFFFFUL / m_u32TickStep * m_u32TickStep;
        /* Add a freq to guarantee minimum wait */
        if ((u32Delay >= tickMax) || ((tickMax - u32Delay) < m_u32TickStep))
        {
            tickEnd = tickMax;
        }
        while ((SysTick_GetTick() - tickStart) < tickEnd)
        {
        }
    }
}

/**
 * @brief This function is called to increment a global variable "u32TickCount".
 * @note  This variable is incremented in SysTick ISR.
 * @param None
 * @retval None
 */
__WEAKDEF void SysTick_IncTick(void)
{
    m_u32TickCount += m_u32TickStep;
}

/**
 * @brief Provides a tick value in millisecond.
 * @param None
 * @retval Tick value
 */
__WEAKDEF uint32_t SysTick_GetTick(void)
{
    return m_u32TickCount;
}

/**
 * @brief Suspend SysTick increment.
 * @param None
 * @retval None
 */
__WEAKDEF void SysTick_Suspend(void)
{
    /* Disable SysTick Interrupt */
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

/**
 * @brief Resume SysTick increment.
 * @param None
 * @retval None
 */
__WEAKDEF void SysTick_Resume(void)
{
    /* Enable SysTick Interrupt */
    SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
}

#ifdef __DEBUG
/**
 * @brief DDL assert error handle function
 * @param [in] file                     Point to the current assert the wrong file.
 * @param [in] line                     Point line assert the wrong file in the current.
 * @retval None
 */
__WEAKDEF void DDL_AssertHandler(const char *file, int line)
{
    /* Users can re-implement this function to print information */
    DDL_Printf("Wrong parameters value: file %s on line %d\r\n", file, line);

    for (;;)
    {
        ;
    }
}
#endif /* __DEBUG */

#if (DDL_PRINT_ENABLE == DDL_ON)

#if (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) || \
    (defined (__ICCARM__)) || (defined (__CC_ARM))
/**
 * @brief  Re-target fputc function.
 * @param  [in] ch
 * @param  [in] f
 * @retval int32_t
 */
int32_t fputc(int32_t ch, FILE *f)
{
    (void)f;  /* Prevent unused argument compilation warning */

    return (Ok == UartPutChar(m_PrintfDevice, (char)ch)) ? ch : -1;
}

#elif defined ( __GNUC__ ) && !defined (__CC_ARM)
/**
 * @brief  Re-target _write function.
 * @param  [in] fd
 * @param  [in] data
 * @param  [in] size
 * @retval int32_t
 */
int32_t _write(int fd, char data[], int32_t size)
{
    int32_t i = -1;

    if (NULL != data)
    {
        (void)fd;  /* Prevent unused argument compilation warning */

        for (i = 0; i < size; i++)
        {
            if (Ok != UartPutChar(m_PrintfDevice, data[i]))
            {
                break;
            }
        }
    }

    return i ? i : -1;
}

#endif

/**
 * @brief  Initialize UART for debug printf function
 * @param  [in] USARTx                  Pointer to USART instance register base
 *         This parameter can be one of the following values:
 *           @arg M4_USART1:            USART unit 1 instance register base
 *           @arg M4_USART2:            USART unit 2 instance register base
 *           @arg M4_USART3:            USART unit 3 instance register base
 *           @arg M4_USART4:            USART unit 4 instance register base
 *           @arg M4_USART5:            USART unit 5 instance register base
 *           @arg M4_USART6:            USART unit 6 instance register base
 *           @arg M4_USART7:            USART unit 7 instance register base
 *           @arg M4_USART8:            USART unit 8 instance register base
 *           @arg M4_USART9:            USART unit 9 instance register base
 *           @arg M4_USART10:           USART unit 10 instance register base
 * @param  [in] u32Baudrate             UART baudrate
 **@param  [in] PortInit                The pointer of printf port initialization function
 * @retval An en_result_t enumeration value:
 *           - Ok: Initialize successfully
 *           - Error: Don't permit write the GPIO configuration register or set baudrate unsuccessfully
 */
en_result_t UART_PrintfInit(M4_USART_TypeDef *USARTx,
                            uint32_t u32Baudrate,
                            void (*PortInit)(void))
{
    en_result_t enRet = Error;

    if (IS_USART_UNIT(USARTx) && (0UL != u32Baudrate) && (NULL != PortInit))
    {
        /* Initialize port */
        PortInit();

        /* Enable clock */
        UART_EnableClk(USARTx);

        /***********************************************************************
         * Configure UART
         ***********************************************************************
         * Baud rate: u32Baudrate
         * Bit direction: LSB
         * Data bits: 8
         * Stop bits: 1
         * Parity: None
         * Sampling bits: 8
         **********************************************************************/
        /* Disbale TX/RX && clear flag */
        WRITE_REG32(USARTx->_CR1, (USART_CR1_CPE | USART_CR1_CORE  | \
                                 USART_CR1_CFE | USART_CR1_CRTOF | \
                                 USART_CR1_CBE | USART_CR1_CWKUP | \
                                 USART_CR1_CLBD));

        /* Set CR1 */
        WRITE_REG32(USARTx->_CR1, (USART_CR1_NFE | USART_CR1_SBS));

        /* Set CR2: reset value */
        WRITE_REG32(USARTx->_CR2, 0x00000600UL);

        /* Set CR3: reset value */
        WRITE_REG32(USARTx->_CR3, 0x00UL);

        /* Set baudrate */
        enRet = UartSetBaudrate(USARTx, u32Baudrate);
        if (Ok == enRet)
        {
            /* Enable TX function */
            SET_REG32_BIT(USARTx->_CR1, USART_CR1_TE);

            m_PrintfDevice = USARTx;
            m_u32PrintfTimeout = (HCLK_VALUE / u32Baudrate);
        }
    }

    return enRet;
}
#endif /* DDL_PRINT_ENABLE */

/**
 * @}
 */

/**
 * @defgroup UTILITY_Local_Functions UTILITY Local Functions
 * @{
 */
#if (DDL_PRINT_ENABLE == DDL_ON)

/**
 * @brief  UART transmit.
 * @param  [in] USARTx                  Pointer to USART instance register base
 *         This parameter can be one of the following values:
 *           @arg M4_USART1:            USART unit 1 instance register base
 *           @arg M4_USART2:            USART unit 2 instance register base
 *           @arg M4_USART3:            USART unit 3 instance register base
 *           @arg M4_USART4:            USART unit 4 instance register base
 *           @arg M4_USART5:            USART unit 5 instance register base
 *           @arg M4_USART6:            USART unit 6 instance register base
 *           @arg M4_USART7:            USART unit 7 instance register base
 *           @arg M4_USART8:            USART unit 8 instance register base
 *           @arg M4_USART9:            USART unit 9 instance register base
 *           @arg M4_USART10:           USART unit 10 instance register base
 * @param  [in] cData                   The data for transmitting
 * @retval An en_result_t enumeration value:
 *           - Ok: Send successfully
 *           - ErrorTimeout: Send timeout
 *           - ErrorInvalidParameter: The parameter USARTx is invalid
 */
static en_result_t UartPutChar(M4_USART_TypeDef *USARTx, char cData)
{
    uint32_t u32TxEmpty;
    en_result_t enRet = ErrorInvalidParameter;
    __IO uint32_t u32Timeout = m_u32PrintfTimeout;

    if (NULL != USARTx)
    {
        /* Wait TX data register empty */
        do
        {
            u32Timeout--;
            u32TxEmpty = READ_REG32_BIT(USARTx->SR, USART_SR_TXE);
        } while ((u32Timeout > 0UL) && (0UL == u32TxEmpty));

        if (0UL != u32TxEmpty)
        {
            WRITE_REG32(USARTx->DR, (uint32_t)cData);
            enRet = Ok;
        }
        else
        {
            enRet = ErrorTimeout;
        }
    }

    return enRet;
}

/**
 * @brief  Set UART baudrate.
 * @param  [in] USARTx                  Pointer to USART instance register base
 *         This parameter can be one of the following values:
 *           @arg M4_USART1:            USART unit 1 instance register base
 *           @arg M4_USART2:            USART unit 2 instance register base
 *           @arg M4_USART3:            USART unit 3 instance register base
 *           @arg M4_USART4:            USART unit 4 instance register base
 *           @arg M4_USART5:            USART unit 5 instance register base
 *           @arg M4_USART6:            USART unit 6 instance register base
 *           @arg M4_USART7:            USART unit 7 instance register base
 *           @arg M4_USART8:            USART unit 8 instance register base
 *           @arg M4_USART9:            USART unit 9 instance register base
 *           @arg M4_USART10:           USART unit 10 instance register base
 * @param  [in] u32Baudrate             UART baudrate
 * @retval An en_result_t enumeration value:
 *           - Ok: Set successfully
 *           - Error: Baudrate set unsuccessfully
 */
static en_result_t UartSetBaudrate(M4_USART_TypeDef *USARTx,
                                    uint32_t u32Baudrate)
{
    uint32_t B;
    uint32_t C;
    uint32_t OVER8;
    float32_t DIV;
    uint32_t DIV_Integer;
    uint32_t u32Prescaler;
    uint32_t u32Pclk;
    uint64_t u64Temp;
    en_result_t enRet = Error;
    uint32_t DIV_Fraction = 0UL;

    if (u32Baudrate > 0UL)
    {
        B = u32Baudrate;
        OVER8 = (0UL != (READ_REG32(USARTx->_CR1) & USART_CR1_OVER8)) ? 1UL : 0UL;
        u32Pclk = (SystemCoreClock >> ((uint32_t)(READ_REG32_BIT(M4_CMU->SCFGR, CMU_SCFGR_PCLK1S) >> CMU_SCFGR_PCLK1S_POS)));

        for (u32Prescaler = 0UL; u32Prescaler <= USART_PR_PSC; u32Prescaler++)
        {
            C = (u32Pclk / (1UL << (u32Prescaler * 2UL)));

            if (C > 0UL)
            {
                /* UART mode baudrate integer calculation formula:      */
                /*      B = C / (8 * (2 - OVER8) * (DIV_Integer + 1))   */
                /*      DIV_Integer = (C / (B * 8 * (2 - OVER8))) - 1   */
                DIV = ((float)C / ((float)B * 8.0F * (2.0F - (float)OVER8))) - 1.0F;
                DIV_Integer = (uint32_t)(DIV);

                if ((DIV > 0.0F) && (DIV_Integer < 0xFFUL))
                {
                    enRet = Ok;
                    if ((DIV - (float32_t)DIV_Integer) > 0.00001F)
                    {
                        /* UART mode baudrate fraction calculation formula:                                 */
                        /*      B = C * (128 + DIV_Fraction) / (8 * (2 - OVER8) * (DIV_Integer + 1) * 256)  */
                        /*      DIV_Fraction = (256 * (8 * (2 - OVER8) * (DIV_Integer + 1) * B) / C) - 128  */
                        u64Temp = (uint64_t)((uint64_t)8UL * ((uint64_t)2UL - (uint64_t)OVER8) * ((uint64_t)DIV_Integer + 1UL) * (uint64_t)B);
                        DIV_Fraction = (uint32_t)(256UL * u64Temp / C - 128UL);
                        if (DIV_Fraction > 0x7FUL)
                        {
                            enRet = Error;
                        }
                    }

                    if (Ok == enRet)
                    {
                        /* Set clock prescaler */
                        WRITE_REG32(USARTx->PR, u32Prescaler);

                        /* Enable or disable baudrate fraction function */
                        MODIFY_REG32(USARTx->_CR1, USART_CR1_FBME, (0UL != DIV_Fraction) ? USART_CR1_FBME : 0UL);

                        /* Set USART_BRR register */
                        WRITE_REG32(USARTx->BRR, ((DIV_Integer << USART_BRR_DIV_INTEGER_POS) + DIV_Fraction));
                        break;
                    }
                }
            }
        }
    }

    return enRet;
}

#endif /* DDL_PRINT_ENABLE */

/**
 * @}
 */

#endif /* DDL_UTILITY_ENABLE */

/**
 * @}
 */

/**
 * @}
 */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
