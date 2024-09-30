/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
 * @file k210_uart_16550.h
 * @brief k210 uart1-uart3 support
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.09.28
 */

#ifndef __INCLUDE_NUTTX_SERIAL_UART_K210_H
#define __INCLUDE_NUTTX_SERIAL_UART_K210_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifdef CONFIG_K210_UART

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* CONFIGURATION ************************************************************/

/* Are any UARTs enabled? */

#undef HAVE_UART
#if defined(CONFIG_K210_UART1) || defined(CONFIG_K210_UART2) || defined(CONFIG_K210_UART3)
#  define HAVE_UART 1
#endif

/* We need to be told the address increment between registers and the
 * register bit width.
 */

#ifndef CONFIG_K210_UART_REGINCR
#  error "CONFIG_K210_UART_REGINCR not defined"
#endif

#if CONFIG_K210_UART_REGINCR != 1 && CONFIG_K210_UART_REGINCR != 2 && CONFIG_K210_UART_REGINCR != 4
#  error "CONFIG_K210_UART_REGINCR not supported"
#endif

#ifndef CONFIG_K210_UART_REGWIDTH
#  error "CONFIG_K210_UART_REGWIDTH not defined"
#endif

#if CONFIG_K210_UART_REGWIDTH != 8 && CONFIG_K210_UART_REGWIDTH != 16 && CONFIG_K210_UART_REGWIDTH != 32
#  error "CONFIG_K210_UART_REGWIDTH not supported"
#endif

#ifndef CONFIG_K210_UART_ADDRWIDTH
#  error "CONFIG_K210_UART_ADDRWIDTH not defined"
#endif

#if CONFIG_K210_UART_ADDRWIDTH != 0 && CONFIG_K210_UART_ADDRWIDTH != 8 && \
    CONFIG_K210_UART_ADDRWIDTH != 16 && CONFIG_K210_UART_ADDRWIDTH != 32 && \
    CONFIG_K210_UART_ADDRWIDTH != 64
#  error "CONFIG_K210_UART_ADDRWIDTH not supported"
#endif

/* If a UART is enabled, then its base address, clock, and IRQ
 * must also be provided
 */

#ifdef CONFIG_K210_UART1
#  ifndef CONFIG_K210_UART1_BASE
#    error "CONFIG_K210_UART1_BASE not provided"
#    undef CONFIG_K210_UART1
#  endif
#  ifndef CONFIG_K210_UART1_CLOCK
#    error "CONFIG_K210_UART1_CLOCK not provided"
#    undef CONFIG_K210_UART1
#  endif
#  ifndef CONFIG_K210_UART1_IRQ
#    error "CONFIG_K210_UART1_IRQ not provided"
#    undef CONFIG_K210_UART1
#  endif
#endif

#ifdef CONFIG_K210_UART2
#  ifndef CONFIG_K210_UART2_BASE
#    error "CONFIG_K210_UART2_BASE not provided"
#    undef CONFIG_K210_UART2
#  endif
#  ifndef CONFIG_K210_UART2_CLOCK
#    error "CONFIG_K210_UART2_CLOCK not provided"
#    undef CONFIG_K210_UART2
#  endif
#  ifndef CONFIG_K210_UART2_IRQ
#    error "CONFIG_K210_UART2_IRQ not provided"
#    undef CONFIG_K210_UART2
#  endif
#endif

#ifdef CONFIG_K210_UART3
#  ifndef CONFIG_K210_UART3_BASE
#    error "CONFIG_K210_UART3_BASE not provided"
#    undef CONFIG_K210_UART3
#  endif
#  ifndef CONFIG_K210_UART3_CLOCK
#    error "CONFIG_K210_UART3_CLOCK not provided"
#    undef CONFIG_K210_UART3
#  endif
#  ifndef CONFIG_K210_UART3_IRQ
#    error "CONFIG_K210_UART3_IRQ not provided"
#    undef CONFIG_K210_UART3
#  endif
#endif

/* Is there a serial console? There should be at most one defined.
 * It could be on any UARTn, n=0,1,2,3
 */

/* Register offsets *********************************************************/

#define UART_RBR_INCR          0 /* (DLAB =0) Receiver Buffer Register */
#define UART_THR_INCR          0 /* (DLAB =0) Transmit Holding Register */
#define UART_DLL_INCR          0 /* (DLAB =1) Divisor Latch LSB */
#define UART_DLM_INCR          1 /* (DLAB =1) Divisor Latch MSB */
#define UART_IER_INCR          1 /* (DLAB =0) Interrupt Enable Register */
#define UART_IIR_INCR          2 /* Interrupt ID Register */
#define UART_FCR_INCR          2 /* FIFO Control Register */
#define UART_LCR_INCR          3 /* Line Control Register */
#define UART_MCR_INCR          4 /* Modem Control Register */
#define UART_LSR_INCR          5 /* Line Status Register */
#define UART_MSR_INCR          6 /* Modem Status Register */
#define UART_SCR_INCR          7 /* Scratch Pad Register */
#define UART_SRT_INCR          39 /* Receive Fifo Trigger Register*/
#define UART_DLF_INCR          48 /* Divisor factor Register*/
#define UART_CPR_INCR          61 /* Component Register */

#define UART_RBR_OFFSET        (CONFIG_K210_UART_REGINCR*UART_RBR_INCR)
#define UART_THR_OFFSET        (CONFIG_K210_UART_REGINCR*UART_THR_INCR)
#define UART_DLL_OFFSET        (CONFIG_K210_UART_REGINCR*UART_DLL_INCR)
#define UART_DLM_OFFSET        (CONFIG_K210_UART_REGINCR*UART_DLM_INCR)
#define UART_IER_OFFSET        (CONFIG_K210_UART_REGINCR*UART_IER_INCR)
#define UART_IIR_OFFSET        (CONFIG_K210_UART_REGINCR*UART_IIR_INCR)
#define UART_FCR_OFFSET        (CONFIG_K210_UART_REGINCR*UART_FCR_INCR)
#define UART_LCR_OFFSET        (CONFIG_K210_UART_REGINCR*UART_LCR_INCR)
#define UART_MCR_OFFSET        (CONFIG_K210_UART_REGINCR*UART_MCR_INCR)
#define UART_LSR_OFFSET        (CONFIG_K210_UART_REGINCR*UART_LSR_INCR)
#define UART_MSR_OFFSET        (CONFIG_K210_UART_REGINCR*UART_MSR_INCR)
#define UART_SCR_OFFSET        (CONFIG_K210_UART_REGINCR*UART_SCR_INCR)
#define UART_SRT_OFFSET        (CONFIG_K210_UART_REGINCR*UART_SRT_INCR)
#define UART_DLF_OFFSET        (CONFIG_K210_UART_REGINCR*UART_DLF_INCR)
#define UART_CPR_OFFSET        (CONFIG_K210_UART_REGINCR*UART_CPR_INCR)

/* Register bit definitions *************************************************/

/* RBR (DLAB =0) Receiver Buffer Register */

#define UART_RBR_MASK                (0xff)    /* Bits 0-7: Oldest received byte in RX FIFO */
                                               /* Bits 8-31: Reserved */

/* THR (DLAB =0) Transmit Holding Register */

#define UART_THR_MASK                (0xff)    /* Bits 0-7: Adds byte to TX FIFO */
                                               /* Bits 8-31: Reserved */

/* DLL (DLAB =1) Divisor Latch LSB */

#define UART_DLL_MASK                (0xff)    /* Bits 0-7: DLL */
                                               /* Bits 8-31: Reserved */

/* DLM (DLAB =1) Divisor Latch MSB */

#define UART_DLM_MASK                (0xff)    /* Bits 0-7: DLM */
                                               /* Bits 8-31: Reserved */

/* IER (DLAB =0) Interrupt Enable Register */

#define UART_IER_ERBFI               (1 << 0)  /* Bit 0: Enable received data available interrupt */
#define UART_IER_ETBEI               (1 << 1)  /* Bit 1: Enable THR empty interrupt */
#define UART_IER_ELSI                (1 << 2)  /* Bit 2: Enable receiver line status interrupt */
#define UART_IER_EDSSI               (1 << 3)  /* Bit 3: Enable MODEM status interrupt */
                                               /* Bits 4-7: Reserved */
#define UART_IER_ALLIE               (0x0f)

/* IIR Interrupt ID Register */

#define UART_IIR_INTSTATUS           (1 << 0)  /* Bit 0:  Interrupt status (active low) */
#define UART_IIR_INTID_SHIFT         (1)       /* Bits 1-3: Interrupt identification */
#define UART_IIR_INTID_MASK          (7 << UART_IIR_INTID_SHIFT)
#  define UART_IIR_INTID_MSI         (0 << UART_IIR_INTID_SHIFT) /* Modem Status */
#  define UART_IIR_INTID_THRE        (1 << UART_IIR_INTID_SHIFT) /* THR Empty Interrupt */
#  define UART_IIR_INTID_RDA         (2 << UART_IIR_INTID_SHIFT) /* Receive Data Available (RDA) */
#  define UART_IIR_INTID_RLS         (3 << UART_IIR_INTID_SHIFT) /* Receiver Line Status (RLS) */
#  define UART_IIR_INTID_CTI         (6 << UART_IIR_INTID_SHIFT) /* Character Time-out Indicator (CTI) */

                                               /* Bits 4-5: Reserved */
#define UART_IIR_FIFOEN_SHIFT        (6)       /* Bits 6-7: RCVR FIFO interrupt */
#define UART_IIR_FIFOEN_MASK         (3 << UART_IIR_FIFOEN_SHIFT)

/* FCR FIFO Control Register */

#define UART_FCR_FIFOEN              (1 << 0)  /* Bit 0:  Enable FIFOs */
#define UART_FCR_RXRST               (1 << 1)  /* Bit 1:  RX FIFO Reset */
#define UART_FCR_TXRST               (1 << 2)  /* Bit 2:  TX FIFO Reset */
#define UART_FCR_DMAMODE             (1 << 3)  /* Bit 3:  DMA Mode Select */
                                               /* Bits 4-5: Reserved */
#define UART_FCR_RXTRIGGER_SHIFT     (6)       /* Bits 6-7: RX Trigger Level */
#define UART_FCR_RXTRIGGER_MASK      (3 << UART_FCR_RXTRIGGER_SHIFT)
#  define UART_FCR_RXTRIGGER_1       (0 << UART_FCR_RXTRIGGER_SHIFT) /*  Trigger level 0 (1 character) */
#  define UART_FCR_RXTRIGGER_4       (1 << UART_FCR_RXTRIGGER_SHIFT) /* Trigger level 1 (4 characters) */
#  define UART_FCR_RXTRIGGER_8       (2 << UART_FCR_RXTRIGGER_SHIFT) /* Trigger level 2 (8 characters) */
#  define UART_FCR_RXTRIGGER_14      (3 << UART_FCR_RXTRIGGER_SHIFT) /* Trigger level 3 (14 characters) */

/* LCR Line Control Register */

#define UART_LCR_WLS_SHIFT           (0)       /* Bit 0-1: Word Length Select */
#define UART_LCR_WLS_MASK            (3 << UART_LCR_WLS_SHIFT)
#  define UART_LCR_WLS_5BIT          (0 << UART_LCR_WLS_SHIFT)
#  define UART_LCR_WLS_6BIT          (1 << UART_LCR_WLS_SHIFT)
#  define UART_LCR_WLS_7BIT          (2 << UART_LCR_WLS_SHIFT)
#  define UART_LCR_WLS_8BIT          (3 << UART_LCR_WLS_SHIFT)
#define UART_LCR_STB                 (1 << 2)  /* Bit 2:  Number of Stop Bits */
#define UART_LCR_PEN                 (1 << 3)  /* Bit 3:  Parity Enable */
#define UART_LCR_EPS                 (1 << 4)  /* Bit 4:  Even Parity Select */
#define UART_LCR_STICKY              (1 << 5)  /* Bit 5:  Stick Parity */
#define UART_LCR_BRK                 (1 << 6)  /* Bit 6: Break Control */
#define UART_LCR_DLAB                (1 << 7)  /* Bit 7: Divisor Latch Access Bit (DLAB) */

/* MCR Modem Control Register */

#define UART_MCR_DTR                 (1 << 0)  /* Bit 0:  DTR Control Source for DTR output */
#define UART_MCR_RTS                 (1 << 1)  /* Bit 1:  Control Source for  RTS output */
#define UART_MCR_OUT1                (1 << 2)  /* Bit 2:  Auxiliary user-defined output 1 */
#define UART_MCR_OUT2                (1 << 3)  /* Bit 3:  Auxiliary user-defined output 2 */
#define UART_MCR_LPBK                (1 << 4)  /* Bit 4:  Loopback Mode Select */
#define UART_MCR_AFCE                (1 << 5)  /* Bit 5:  Auto Flow Control Enable */
                                               /* Bit 6-7: Reserved */

/* LSR Line Status Register */

#define UART_LSR_DR                  (1 << 0)  /* Bit 0:  Data Ready */
#define UART_LSR_OE                  (1 << 1)  /* Bit 1:  Overrun Error */
#define UART_LSR_PE                  (1 << 2)  /* Bit 2:  Parity Error */
#define UART_LSR_FE                  (1 << 3)  /* Bit 3:  Framing Error */
#define UART_LSR_BI                  (1 << 4)  /* Bit 4:  Break Interrupt */
#define UART_LSR_THRE                (1 << 5)  /* Bit 5:  Transmitter Holding Register Empty */
#define UART_LSR_TEMT                (1 << 6)  /* Bit 6:  Transmitter Empty */
#define UART_LSR_RXFE                (1 << 7)  /* Bit 7:  Error in RX FIFO (RXFE) */

#define UART_INTERRUPT_SEND 0x02U
#define UART_INTERRUPT_RECEIVE 0x04U
#define UART_INTERRUPT_CHARACTER_TIMEOUT 0x0CU

/* SCR Scratch Pad Register */

#define UART_SCR_MASK                (0xff)    /* Bits 0-7: SCR data */

/****************************************************************************
 * Public Types
 ****************************************************************************/

#if CONFIG_K210_UART_REGWIDTH == 8
typedef uint8_t uart_datawidth_t;
#elif CONFIG_K210_UART_REGWIDTH == 16
typedef uint16_t uart_datawidth_t;
#elif CONFIG_K210_UART_REGWIDTH == 32
typedef uint32_t uart_datawidth_t;
#endif

#if CONFIG_K210_UART_ADDRWIDTH == 0
typedef uintptr_t uart_addrwidth_t;
#elif CONFIG_K210_UART_ADDRWIDTH == 8
typedef uint8_t uart_addrwidth_t;
#elif CONFIG_K210_UART_ADDRWIDTH == 16
typedef uint16_t uart_addrwidth_t;
#elif CONFIG_K210_UART_ADDRWIDTH == 32
typedef uint32_t uart_addrwidth_t;
#elif CONFIG_K210_UART_ADDRWIDTH == 64
typedef uint64_t uart_addrwidth_t;
#endif

struct k210_uart_s
{
  uart_addrwidth_t uartbase;  /* Base address of UART registers */
#ifndef CONFIG_K210_UART_SUPRESS_CONFIG
  uint32_t         baud;      /* Configured baud */
  uint32_t         uartclk;   /* UART clock frequency */
#endif
  uart_datawidth_t ier;       /* Saved IER value */
  uint8_t          irq;       /* IRQ associated with this UART */
#ifndef CONFIG_K210_UART_SUPRESS_CONFIG
  uint8_t          parity;    /* 0=none, 1=odd, 2=even */
  uint8_t          bits;      /* Number of bits (7 or 8) */
  int             stopbits2; /* true: Configure with 2 stop bits instead of 1 */
#if defined(CONFIG_SERIAL_IFLOWCONTROL) || defined(CONFIG_SERIAL_OFLOWCONTROL)
  bool             flow;      /* flow control (RTS/CTS) enabled */
#endif
#endif
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Functions Definitions
 ****************************************************************************/

/****************************************************************************
 * Name: uart_getreg(), uart_putreg(), uart_ioctl()
 *
 * Description:
 *   These functions must be provided by the processor-specific code in order
 *   to correctly access registers
 *   uart_ioctl() is optional to provide custom IOCTLs
 *
 ****************************************************************************/

#ifndef CONFIG_SERIAL_UART_ARCH_MMIO
uart_datawidth_t uart_getreg(uart_addrwidth_t base, unsigned int offset);
void uart_putreg(uart_addrwidth_t base,
                 unsigned int offset,
                 uart_datawidth_t value);
#endif

struct file;  /* Forward reference */
int uart_ioctl(struct file *filep, int cmd, unsigned long arg);

void k210_uart_register(void);
#endif /* CONFIG_K210_UART */
#endif /* __INCLUDE_NUTTX_SERIAL_UART_16550_H */
