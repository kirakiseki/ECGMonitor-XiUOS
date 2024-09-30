/**
 *******************************************************************************
 * @file  hc32_common.h
 * @brief This file contains the common part of the HC32 series.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2020-06-12       Yangjp          First version
   2020-09-07       Yangjp          Add the precompiled configuration of ARM compiler V6
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
#ifndef __HC32_COMMON_H__
#define __HC32_COMMON_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <stddef.h>
#include <string.h>

//configure the hc32 nuttx adapter
#define HC32F4A0 1
#define USE_DDL_DRIVER 1

#ifdef CONFIG_FS_PROCFS
#  ifdef CONFIG_NSH_PROC_MOUNTPOINT
#    define HC32_PROCFS_MOUNTPOINT CONFIG_NSH_PROC_MOUNTPOINT
#  else
#    define HC32_PROCFS_MOUNTPOINT "/proc"
#  endif
#endif


#define getreg32(a)      (*(volatile uint32_t *)(a))
#define putreg32(v,a)    (*(volatile uint32_t *)(a) = (v))
#define getreg16(a)      (*(volatile uint16_t *)(a))
#define putreg16(v,a)    (*(volatile uint16_t *)(a) = (v))


extern uint32_t SystemCoreClock;        /*!< System clock frequency (Core clock) */
extern uint32_t HRC_VALUE;              /*!< HRC frequency */

/**
 * @brief Clock setup macro definition
 */
#define CLOCK_SETTING_NONE              0U  /*!< User provides own clock setting in application */
#define CLOCK_SETTING_CMSIS             1U

#define HRC_FREQ_MON()                  (*((volatile unsigned int*)(0x40010684UL)))

/**
 * @addtogroup HC32F4A0_System_Clock_Source
 * @{
 */
#if !defined (HRC_16MHz_VALUE)
    #define HRC_16MHz_VALUE             ((uint32_t)16000000UL)  /*!< Internal high speed RC freq.(16MHz) */
#endif

#if !defined (HRC_20MHz_VALUE)
    #define HRC_20MHz_VALUE             ((uint32_t)20000000UL)  /*!< Internal high speed RC freq.(20MHz) */
#endif

#if !defined (MRC_VALUE)
#define MRC_VALUE                       ((uint32_t)8000000UL)   /*!< Internal middle speed RC freq.(8MHz) */
#endif

#if !defined (LRC_VALUE)
    #define LRC_VALUE                   ((uint32_t)32768UL)     /*!< Internal low speed RC freq.(32.768KHz) */
#endif

#if !defined (RTCLRC_VALUE)
    #define RTCLRC_VALUE                ((uint32_t)32768UL)     /*!< Internal RTC low speed RC freq.(32.768KHz) */
#endif

#if !defined (SWDTLRC_VALUE)
    #define SWDTLRC_VALUE               ((uint32_t)10000UL)     /*!< External low speed OSC freq.(10KHz) */
#endif

#if !defined (XTAL_VALUE)
    #define XTAL_VALUE                  ((uint32_t)8000000UL)   /*!< External high speed OSC freq.(8MHz) */
#endif

#if !defined (XTAL32_VALUE)
    #define XTAL32_VALUE                ((uint32_t)32768UL)     /*!< External low speed OSC freq.(32.768KHz) */
#endif

#if !defined (HCLK_VALUE)
extern uint32_t SystemCoreClock;
#define HCLK_VALUE      (SystemCoreClock >> ((M4_CMU->SCFGR & CMU_SCFGR_HCLKS) >> CMU_SCFGR_HCLKS_POS))
#endif

#define HC32_SYSCLK_FREQUENCY SystemCoreClock
#define HC32_HCLK_FREQUENCY HCLK_VALUE


/**
 * @addtogroup CMSIS
 * @{
 */

/**
 * @addtogroup HC32_Common_Part
 * @{
 */

/**
 * @brief HC32 Common Device Include
 */
#include "hc32f4a0.h"

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/
/**
 * @defgroup HC32_Common_Global_Types HC32 Common Global Types
 * @{
 */

/**
 * @brief Single precision floating point number (4 byte)
 */
typedef float float32_t;

/**
 * @brief Double precision floating point number (8 byte)
 */
typedef double float64_t;

/**
 * @brief Function pointer type to void/void function
 */
typedef void (*func_ptr_t)(void);

/**
 * @brief Function pointer type to void/uint8_t function
 */
typedef void (*func_ptr_arg1_t)(uint8_t);

/**
 * @brief Functional state
 */
typedef enum
{
    Disable = 0U,
    Enable  = 1U,
} en_functional_state_t;

/* Check if it is a functional state */
#define IS_FUNCTIONAL_STATE(state)      (((state) == Disable) || ((state) == Enable))

/**
 * @brief Flag status
 */
typedef enum
{
    Reset = 0U,
    Set   = 1U,
} en_flag_status_t, en_int_status_t;

/**
 * @brief Generic error codes
 */
typedef enum
{
    Ok                       = 0U,  /*!< No error */
    Error                    = 1U,  /*!< Non-specific error code */
    ErrorAddressAlignment    = 2U,  /*!< Address alignment does not match */
    ErrorAccessRights        = 3U,  /*!< Wrong mode (e.g. user/system) mode is set */
    ErrorInvalidParameter    = 4U,  /*!< Provided parameter is not valid */
    ErrorOperationInProgress = 5U,  /*!< A conflicting or requested operation is still in progress */
    ErrorInvalidMode         = 6U,  /*!< Operation not allowed in current mode */
    ErrorUninitialized       = 7U,  /*!< Module (or part of it) was not initialized properly */
    ErrorBufferEmpty         = 8U,  /*!< Circular buffer can not be read because the buffer is empty */
    ErrorBufferFull          = 9U,  /*!< Circular buffer can not be written because the buffer is full */
    ErrorTimeout             = 10U, /*!< Time Out error occurred (e.g. I2C arbitration lost, Flash time-out, etc.) */
    ErrorNotReady            = 11U, /*!< A requested final state is not reached */
    OperationInProgress      = 12U, /*!< Indicator for operation in progress (e.g. ADC conversion not finished, DMA channel used, etc.) */
} en_result_t;

/**
 * @}
 */

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
/**
 * @defgroup HC32_Common_Global_Macros HC32 Common Global Macros
 * @{
 */

/**
 * @brief Compiler Macro Definitions
 */
#ifndef __UNUSED
  #define __UNUSED                      __attribute__((unused))
#endif /* __UNUSED */

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #ifndef __WEAKDEF
    #define __WEAKDEF                   __attribute__((weak))
  #endif /* __WEAKDEF */
  #ifndef __ALIGN_BEGIN
    #define __ALIGN_BEGIN               __attribute__((aligned(4)))
  #endif /* __ALIGN_BEGIN */
  #ifndef __NOINLINE
    #define __NOINLINE                  __attribute__((noinline))
  #endif /* __NOINLINE */
  #ifndef __RAM_FUNC
    #define __RAM_FUNC                  __attribute__((long_call, section(".ramfunc")))
    /* Usage: void __RAM_FUNC foo(void) */
  #endif /* __RAM_FUNC */
  #ifndef __NO_INIT
    #define __NO_INIT
  #endif /* __NO_INIT */
#elif defined ( __GNUC__ ) && !defined (__CC_ARM) /*!< GNU Compiler */
  #ifndef __WEAKDEF
    #define __WEAKDEF                   __attribute__((weak))
  #endif /* __WEAKDEF */
  #ifndef __ALIGN_BEGIN
    #define __ALIGN_BEGIN               __attribute__((aligned (4)))
  #endif /* __ALIGN_BEGIN */
  #ifndef __NOINLINE
    #define __NOINLINE                  __attribute__((noinline))
  #endif /* __NOINLINE */
  #ifndef __RAM_FUNC
    #define __RAM_FUNC                  __attribute__((long_call, section(".ramfunc")))
    /* Usage: void __RAM_FUNC foo(void) */
  #endif /* __RAM_FUNC */
  #ifndef __NO_INIT
    #define __NO_INIT                   __attribute__((section(".noinit")))
  #endif /* __NO_INIT */
#elif defined (__ICCARM__)                /*!< IAR Compiler */
  #ifndef __WEAKDEF
    #define __WEAKDEF                   __weak
  #endif /* __WEAKDEF */
  #ifndef __ALIGN_BEGIN
    #define __ALIGN_BEGIN               _Pragma("data_alignment=4")
  #endif /* __ALIGN_BEGIN */
  #ifndef __NOINLINE
    #define __NOINLINE                  _Pragma("optimize = no_inline")
  #endif /* __NOINLINE */
  #ifndef __RAM_FUNC
    #define __RAM_FUNC                  __ramfunc
  #endif /* __RAM_FUNC */
#ifndef __NO_INIT
    #define __NO_INIT                   __no_init
#endif /* __NO_INIT */
#elif defined (__CC_ARM)                /*!< ARM Compiler */
  #ifndef __WEAKDEF
    #define __WEAKDEF                   __attribute__((weak))
  #endif /* __WEAKDEF */
  #ifndef __ALIGN_BEGIN
    #define __ALIGN_BEGIN               __align(4)
  #endif /* __ALIGN_BEGIN */
  #ifndef __NOINLINE
    #define __NOINLINE                  __attribute__((noinline))
  #endif /* __NOINLINE */
  #ifndef __NO_INIT
    #define __NO_INIT
  #endif /* __NO_INIT */
    /* RAM functions are defined using the toolchain options.
    Functions that are executed in RAM should reside in a separate source module.
    Using the 'Options for File' dialog you can simply change the 'Code / Const'
    area of a module to a memory space in physical RAM. */
    #define __RAM_FUNC
#else
    #error  "unsupported compiler!!"
#endif

/**
 * @defgroup Extend_Macro_Definitions Extend Macro Definitions
 * @{
 */
/* Decimal to BCD */
#define DEC2BCD(x)                      ((((x) / 10U) << 4U) + ((x) % 10U))

/* BCD to decimal */
#define BCD2DEC(x)                      ((((x) >> 4U) * 10U) + ((x) & 0x0FU))

/* Returns the minimum value out of two values */
#define MIN(x, y)                       ((x) < (y) ? (x) : (y))

/* Returns the maximum value out of two values */
#define MAX(x, y)                       ((x) > (y) ? (x) : (y))

/* Returns the dimension of an array */
#define ARRAY_SZ(x)                     ((sizeof(x)) / (sizeof((x)[0])))
/**
 * @}
 */

/**
 * @defgroup Address_Align Address Align
 * @{
 */
#define IS_ADDRESS_ALIGN(addr, align)   (0UL == (((uint32_t)(addr)) & (((uint32_t)(align)) - 1UL)))
#define IS_ADDRESS_ALIGN_HALFWORD(addr) (0UL == (((uint32_t)(addr)) & 0x1UL))
#define IS_ADDRESS_ALIGN_WORD(addr)     (0UL == (((uint32_t)(addr)) & 0x3UL))
/**
 * @}
 */

/**
 * @defgroup Register_Macro_Definitions Register Macro Definitions
 * @{
 */
#define RW_MEM8(addr)                   (*(volatile uint8_t *)(addr))
#define RW_MEM16(addr)                  (*(volatile uint16_t *)(addr))
#define RW_MEM32(addr)                  (*(volatile uint32_t *)(addr))

#define SET_REG8_BIT(REG, BIT)          ((REG) |= ((uint8_t)(BIT)))
#define SET_REG16_BIT(REG, BIT)         ((REG) |= ((uint16_t)(BIT)))
#define SET_REG32_BIT(REG, BIT)         ((REG) |= ((uint32_t)(BIT)))

#define CLEAR_REG8_BIT(REG, BIT)        ((REG) &= ((uint8_t)(~((uint8_t)(BIT)))))
#define CLEAR_REG16_BIT(REG, BIT)       ((REG) &= ((uint16_t)(~((uint16_t)(BIT)))))
#define CLEAR_REG32_BIT(REG, BIT)       ((REG) &= ((uint32_t)(~((uint32_t)(BIT)))))

#define READ_REG8_BIT(REG, BIT)         ((REG) & ((uint8_t)(BIT)))
#define READ_REG16_BIT(REG, BIT)        ((REG) & ((uint16_t)(BIT)))
#define READ_REG32_BIT(REG, BIT)        ((REG) & ((uint32_t)(BIT)))

#define CLEAR_REG8(REG)                 ((REG) = ((uint8_t)(0U)))
#define CLEAR_REG16(REG)                ((REG) = ((uint16_t)(0U)))
#define CLEAR_REG32(REG)                ((REG) = ((uint32_t)(0UL)))

#define WRITE_REG8(REG, VAL)            ((REG) = ((uint8_t)(VAL)))
#define WRITE_REG16(REG, VAL)           ((REG) = ((uint16_t)(VAL)))
#define WRITE_REG32(REG, VAL)           ((REG) = ((uint32_t)(VAL)))

#define READ_REG8(REG)                  (REG)
#define READ_REG16(REG)                 (REG)
#define READ_REG32(REG)                 (REG)

#define MODIFY_REG8(REGS, CLEARMASK, SETMASK)   (WRITE_REG8((REGS), (((READ_REG8((REGS))) & ((uint8_t)(~((uint8_t)(CLEARMASK))))) | ((uint8_t)(SETMASK) & (uint8_t)(CLEARMASK)))))
#define MODIFY_REG16(REGS, CLEARMASK, SETMASK)  (WRITE_REG16((REGS), (((READ_REG16((REGS))) & ((uint16_t)(~((uint16_t)(CLEARMASK))))) | ((uint16_t)(SETMASK) & (uint16_t)(CLEARMASK)))))
#define MODIFY_REG32(REGS, CLEARMASK, SETMASK)  (WRITE_REG32((REGS), (((READ_REG32((REGS))) & ((uint32_t)(~((uint32_t)(CLEARMASK))))) | ((uint32_t)(SETMASK) & (uint32_t)(CLEARMASK)))))
/**
 * @}
 */

/**
 * @}
 */

/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
 * Global function prototypes (definition in C source)
 ******************************************************************************/

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __HC32_COMMON_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
