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
 * @file k210_uart.c
 * @brief k210 uart1-uart3 support
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.09.28
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/serial/serial.h>
#include <nuttx/fs/ioctl.h>
#include "k210_uart.h"

#ifdef CONFIG_SERIAL_TERMIOS
#  include <termios.h>
#endif

#ifdef CONFIG_K210_UART


/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int  k210_uart_setup(FAR struct uart_dev_s *dev);
static void k210_uart_shutdown(FAR struct uart_dev_s *dev);
static int  k210_uart_attach(FAR struct uart_dev_s *dev);
static void k210_uart_detach(FAR struct uart_dev_s *dev);
static int  k210_uart_interrupt(int irq, FAR void *context, FAR void *arg);
static int  k210_uart_ioctl(FAR struct file *filep, int cmd, unsigned long arg);
static int  k210_uart_receive(FAR struct uart_dev_s *dev, unsigned int *status);
static void k210_uart_rxint(FAR struct uart_dev_s *dev, bool enable);
static bool k210_uart_rxavailable(FAR struct uart_dev_s *dev);
#ifdef CONFIG_SERIAL_IFLOWCONTROL
static bool k210_uart_rxflowcontrol(struct uart_dev_s *dev,
                                 unsigned int nbuffered, bool upper);
#endif
#ifdef CONFIG_SERIAL_TXDMA
static void k210_uart_dmasend(FAR struct uart_dev_s *dev);
static void k210_uart_dmatxavail(FAR struct uart_dev_s *dev);
#endif
#ifdef CONFIG_SERIAL_RXDMA
static void k210_uart_dmareceive(FAR struct uart_dev_s *dev);
static void k210_uart_dmarxfree(FAR struct uart_dev_s *dev);
#endif
static void k210_uart_send(FAR struct uart_dev_s *dev, int ch);
static void k210_uart_txint(FAR struct uart_dev_s *dev, bool enable);
static bool k210_uart_txready(FAR struct uart_dev_s *dev);
static bool k210_uart_txempty(FAR struct uart_dev_s *dev);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct uart_ops_s g_uart_ops =
{
  .setup          = k210_uart_setup,
  .shutdown       = k210_uart_shutdown,
  .attach         = k210_uart_attach,
  .detach         = k210_uart_detach,
  .ioctl          = k210_uart_ioctl,
  .receive        = k210_uart_receive,
  .rxint          = k210_uart_rxint,
  .rxavailable    = k210_uart_rxavailable,
#ifdef CONFIG_SERIAL_IFLOWCONTROL
  .rxflowcontrol  = k210_uart_rxflowcontrol,
#endif
#ifdef CONFIG_SERIAL_TXDMA
  .dmasend        = k210_uart_dmasend,
#endif
#ifdef CONFIG_SERIAL_RXDMA
  .dmareceive     = k210_uart_dmareceive,
  .dmarxfree      = k210_uart_dmarxfree,
#endif
#ifdef CONFIG_SERIAL_TXDMA
  .dmatxavail     = k210_uart_dmatxavail,
#endif
  .send           = k210_uart_send,
  .txint          = k210_uart_txint,
  .txready        = k210_uart_txready,
  .txempty        = k210_uart_txempty,
};

/* I/O buffers */

#ifdef CONFIG_K210_UART1
static char g_uart1rxbuffer[CONFIG_K210_UART1_RXBUFSIZE];
static char g_uart1txbuffer[CONFIG_K210_UART1_TXBUFSIZE];
#endif
#ifdef CONFIG_K210_UART2
static char g_uart2rxbuffer[CONFIG_K210_UART2_RXBUFSIZE];
static char g_uart2txbuffer[CONFIG_K210_UART2_TXBUFSIZE];
#endif
#ifdef CONFIG_K210_UART3
static char g_uart3rxbuffer[CONFIG_K210_UART3_RXBUFSIZE];
static char g_uart3txbuffer[CONFIG_K210_UART3_TXBUFSIZE];
#endif

/* This describes the state of the uart1 port. */

#ifdef CONFIG_K210_UART1
static struct k210_uart_s g_uart1priv =
{
  .uartbase       = CONFIG_K210_UART1_BASE,
#ifndef CONFIG_K210_UART_SUPRESS_CONFIG
  .baud           = CONFIG_K210_UART1_BAUD,
  .uartclk        = CONFIG_K210_UART1_CLOCK,
#endif
  .irq            = CONFIG_K210_UART1_IRQ,
#ifndef CONFIG_K210_UART_SUPRESS_CONFIG
  .parity         = CONFIG_K210_UART1_PARITY,
  .bits           = CONFIG_K210_UART1_BITS,
  .stopbits2      = CONFIG_K210_UART1_2STOP,
#if defined(CONFIG_K210_UART1_IFLOWCONTROL) || defined(CONFIG_K210_UART1_OFLOWCONTROL)
  .flow           = true,
#endif
#endif
};

static uart_dev_t g_uart1port =
{
  .recv     =
  {
    .size   = CONFIG_K210_UART1_RXBUFSIZE,
    .buffer = g_uart1rxbuffer,
  },
  .xmit     =
  {
    .size   = CONFIG_K210_UART1_TXBUFSIZE,
    .buffer = g_uart1txbuffer,
  },
  .ops      = &g_uart_ops,
  .priv     = &g_uart1priv,
};
#endif

/* This describes the state of the uart2 port. */

#ifdef CONFIG_K210_UART2
static struct k210_uart_s g_uart2priv =
{
  .uartbase       = CONFIG_K210_UART2_BASE,
#ifndef CONFIG_K210_UART_SUPRESS_CONFIG
  .baud           = CONFIG_K210_UART2_BAUD,
  .uartclk        = CONFIG_K210_UART2_CLOCK,
#endif
  .irq            = CONFIG_K210_UART2_IRQ,
#ifndef CONFIG_K210_UART_SUPRESS_CONFIG
  .parity         = CONFIG_K210_UART2_PARITY,
  .bits           = CONFIG_K210_UART2_BITS,
  .stopbits2      = CONFIG_K210_UART2_2STOP,
#if defined(CONFIG_K210_UART2_IFLOWCONTROL) || defined(CONFIG_K210_UART2_OFLOWCONTROL)
  .flow           = true,
#endif
#endif
};

static uart_dev_t g_uart2port =
{
  .recv     =
  {
    .size   = CONFIG_K210_UART2_RXBUFSIZE,
    .buffer = g_uart2rxbuffer,
  },
  .xmit     =
  {
    .size   = CONFIG_K210_UART2_TXBUFSIZE,
    .buffer = g_uart2txbuffer,
  },
  .ops      = &g_uart_ops,
  .priv     = &g_uart2priv,
};

#endif

/* This describes the state of the uart1 port. */

#ifdef CONFIG_K210_UART3
static struct k210_uart_s g_uart3priv =
{
  .uartbase       = CONFIG_K210_UART3_BASE,
#ifndef CONFIG_K210_UART_SUPRESS_CONFIG
  .baud           = CONFIG_K210_UART3_BAUD,
  .uartclk        = CONFIG_K210_UART3_CLOCK,
#endif
  .irq            = CONFIG_K210_UART3_IRQ,
#ifndef CONFIG_K210_UART_SUPRESS_CONFIG
  .parity         = CONFIG_K210_UART3_PARITY,
  .bits           = CONFIG_K210_UART3_BITS,
  .stopbits2      = CONFIG_K210_UART3_2STOP,
#if defined(CONFIG_K210_UART3_IFLOWCONTROL) || defined(CONFIG_K210_UART3_OFLOWCONTROL)
  .flow           = true,
#endif
#endif
};

static uart_dev_t g_uart3port =
{
  .recv     =
  {
    .size   = CONFIG_K210_UART3_RXBUFSIZE,
    .buffer = g_uart3rxbuffer,
  },
  .xmit     =
  {
    .size   = CONFIG_K210_UART3_TXBUFSIZE,
    .buffer = g_uart3txbuffer,
  },
  .ops      = &g_uart_ops,
  .priv     = &g_uart3priv,
};

#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: k210_uart_serialin
 ****************************************************************************/

static inline uart_datawidth_t k210_uart_serialin(FAR struct k210_uart_s *priv,
                                               int offset)
{
#ifdef CONFIG_SERIAL_UART_ARCH_MMIO
  return *((FAR volatile uart_datawidth_t *)(priv->uartbase + offset));
#else
  return uart_getreg(priv->uartbase, offset);
#endif
}

/****************************************************************************
 * Name: k210_uart_serialout
 ****************************************************************************/

static inline void k210_uart_serialout(FAR struct k210_uart_s *priv, int offset,
                                    uart_datawidth_t value)
{
#ifdef CONFIG_SERIAL_UART_ARCH_MMIO
  *((FAR volatile uart_datawidth_t *)(priv->uartbase + offset)) = value;
#else
  uart_putreg(priv->uartbase, offset, value);
#endif
}

/****************************************************************************
 * Name: k210_uart_disableuartint
 ****************************************************************************/

static inline void k210_uart_disableuartint(FAR struct k210_uart_s *priv,
                                         FAR uart_datawidth_t *ier)
{
  if (ier)
    {
      *ier = priv->ier & UART_IER_ALLIE;
    }

  priv->ier &= ~UART_IER_ALLIE;
  k210_uart_serialout(priv, UART_IER_OFFSET, priv->ier);
}

/****************************************************************************
 * Name: k210_uart_restoreuartint
 ****************************************************************************/

static inline void k210_uart_restoreuartint(FAR struct k210_uart_s *priv,
                                         uint32_t ier)
{
  priv->ier |= ier & UART_IER_ALLIE;
  k210_uart_serialout(priv, UART_IER_OFFSET, priv->ier);
}

/****************************************************************************
 * Name: k210_uart_enablebreaks
 ****************************************************************************/

static inline void k210_uart_enablebreaks(FAR struct k210_uart_s *priv,
                                       bool enable)
{
  uint32_t lcr = k210_uart_serialin(priv, UART_LCR_OFFSET);

  if (enable)
    {
      lcr |= UART_LCR_BRK;
    }
  else
    {
      lcr &= ~UART_LCR_BRK;
    }

  k210_uart_serialout(priv, UART_LCR_OFFSET, lcr);
}

/****************************************************************************
 * Name: k210_uart_divisor
 *
 * Description:
 *   Select a divider to produce the BAUD from the UART_CLK.
 *
 *     BAUD = UART_CLK / (16 * DL), or
 *     DIV  = UART_CLK / BAUD / 16
 *
 *   Ignoring the fractional divider for now.
 *
 ****************************************************************************/

#ifndef CONFIG_K210_UART_SUPRESS_CONFIG
static inline uint32_t k210_uart_divisor(FAR struct k210_uart_s *priv)
{
  return (priv->uartclk / (uint32_t)priv->baud);
//  return (priv->uartclk + (priv->baud << 3)) / (priv->baud << 4);
}
#endif

/****************************************************************************
 * Name: k210_uart_setup
 *
 * Description:
 *   Configure the UART baud, bits, parity, fifos, etc. This
 *   method is called the first time that the serial port is
 *   opened.
 *
 ****************************************************************************/

static int k210_uart_setup(FAR struct uart_dev_s *dev)
{
#ifndef CONFIG_K210_UART_SUPRESS_CONFIG
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;
  uint16_t div;
  uint8_t dlh, dll, dlf;
  uint32_t lcr;
#if defined(CONFIG_SERIAL_IFLOWCONTROL) || defined(CONFIG_SERIAL_OFLOWCONTROL)
  uint32_t mcr;
#endif

  /* Clear fifos */

  k210_uart_serialout(priv, UART_FCR_OFFSET,
                   (UART_FCR_RXRST | UART_FCR_TXRST));

  /* Set trigger */

  k210_uart_serialout(priv, UART_FCR_OFFSET,
                   (UART_FCR_FIFOEN | UART_FCR_RXTRIGGER_8));

  /* Set up the IER */

  priv->ier = k210_uart_serialin(priv, UART_IER_OFFSET);

  /* Set up the LCR */

  lcr = 0;
  switch (priv->bits)
    {
      case 5 :
        lcr |= UART_LCR_WLS_5BIT;
        break;

      case 6 :
        lcr |= UART_LCR_WLS_6BIT;
        break;

      case 7 :
        lcr |= UART_LCR_WLS_7BIT;
        break;

      default:
      case 8 :
        lcr |= UART_LCR_WLS_8BIT;
        break;
    }

  if (priv->stopbits2)
    {
      lcr |= UART_LCR_STB;
    }

  if (priv->parity == 1)
    {
      lcr |= UART_LCR_PEN;
    }
  else if (priv->parity == 2)
    {
      lcr |= (UART_LCR_PEN | UART_LCR_EPS);
    }

  /* Enter DLAB=1 */

  k210_uart_serialout(priv, UART_LCR_OFFSET, (lcr | UART_LCR_DLAB));

  /* Set the BAUD divisor */

  div = k210_uart_divisor(priv);
  dlh = div >> 12;
  dll = (div - (dlh << 12)) / 16;
  dlf = div - (dlh << 12) - dll * 16;

  k210_uart_serialout(priv, UART_DLM_OFFSET, dlh);
  k210_uart_serialout(priv, UART_DLL_OFFSET, dll);
  k210_uart_serialout(priv, UART_DLF_OFFSET, dlf);

  /* Clear DLAB */

  k210_uart_serialout(priv, UART_LCR_OFFSET, lcr);

  /* Configure the FIFOs */

  k210_uart_serialout(priv, UART_FCR_OFFSET,
                   (UART_FCR_RXTRIGGER_8 | UART_FCR_TXRST | UART_FCR_RXRST |
                    UART_FCR_FIFOEN));

  /* Set up the auto flow control */

#if defined(CONFIG_SERIAL_IFLOWCONTROL) || defined(CONFIG_SERIAL_OFLOWCONTROL)
  mcr = k210_uart_serialin(priv, UART_MCR_OFFSET);
  if (priv->flow)
    {
      mcr |= UART_MCR_AFCE;
    }
  else
    {
      mcr &= ~UART_MCR_AFCE;
    }

  mcr |= UART_MCR_RTS;

  k210_uart_serialout(priv, UART_MCR_OFFSET, mcr);

  k210_uart_serialout(priv, UART_SRT_OFFSET, 0x0);

#endif /* defined(CONFIG_SERIAL_IFLOWCONTROL) || defined(CONFIG_SERIAL_OFLOWCONTROL) */

#endif
  return OK;
}

/****************************************************************************
 * Name: k210_uart_shutdown
 *
 * Description:
 *   Disable the UART.  This method is called when the serial
 *   port is closed
 *
 ****************************************************************************/

static void k210_uart_shutdown(struct uart_dev_s *dev)
{
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;
  k210_uart_disableuartint(priv, NULL);
}

/****************************************************************************
 * Name: k210_uart_attach
 *
 * Description:
 *   Configure the UART to operation in interrupt driven mode.  This method
 *   is called when the serial port is opened.  Normally, this is just after
 *   the setup() method is called, however, the serial console may operate in
 *   a non-interrupt driven mode during the boot phase.
 *
 *   RX and TX interrupts are not enabled when by the attach method (unless
 *   the hardware supports multiple levels of interrupt enabling).  The RX
 *   and TX interrupts are not enabled until the txint() and rxint() methods
 *   are called.
 *
 ****************************************************************************/

static int k210_uart_attach(struct uart_dev_s *dev)
{
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;
  int ret;

  /* Attach and enable the IRQ */

  ret = irq_attach(priv->irq, k210_uart_interrupt, dev);
#ifndef CONFIG_ARCH_NOINTC
  if (ret == OK)
    {
      /* Enable the interrupt (RX and TX interrupts are still disabled
       * in the UART
       */

      up_enable_irq(priv->irq);
    }
#endif

  return ret;
}

/****************************************************************************
 * Name: k210_uart_detach
 *
 * Description:
 *   Detach UART interrupts.  This method is called when the serial port is
 *   closed normally just before the shutdown method is called.
 *   The exception is the serial console which is never shutdown.
 *
 ****************************************************************************/

static void k210_uart_detach(FAR struct uart_dev_s *dev)
{
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;

  up_disable_irq(priv->irq);
  irq_detach(priv->irq);
}

/****************************************************************************
 * Name: k210_uart_interrupt
 *
 * Description:
 *   This is the UART interrupt handler.  It will be invoked when an
 *   interrupt received on the 'irq'  It should call uart_transmitchars or
 *   uart_receivechar to perform the appropriate data transfers.  The
 *   interrupt handling logic must be able to map the 'irq' number into the
 *   appropriate k210_uart_s structure in order to call these functions.
 *
 ****************************************************************************/

static int k210_uart_interrupt(int irq, FAR void *context, FAR void *arg)
{
  FAR struct uart_dev_s *dev = (struct uart_dev_s *)arg;
  FAR struct k210_uart_s *priv;
  uint32_t status;
  int passes;
  uint8_t v_int_status;

  DEBUGASSERT(dev != NULL && dev->priv != NULL);
  priv = (FAR struct k210_uart_s *)dev->priv;

  /* Loop until there are no characters to be transferred or,
   * until we have been looping for a long time.
   */

  for (passes = 0; passes < 256; passes++)
    {
      /* Get the current UART status and check for loop
       * termination conditions
       */

      status = k210_uart_serialin(priv, UART_IIR_OFFSET);

      if (status == 0)
      {
        break;
      }
      v_int_status = status & 0xF;

      /* The UART_IIR_INTSTATUS bit should be zero if there are pending
       * interrupts
       */
      if(v_int_status == UART_INTERRUPT_SEND)
      {
        uart_xmitchars(dev);
        break;
      }

      else if (v_int_status == UART_INTERRUPT_RECEIVE || v_int_status == UART_INTERRUPT_CHARACTER_TIMEOUT)
      {
        uart_recvchars(dev);
        break;
      }
    }

  return OK;
}

/****************************************************************************
 * Name: k210_uart_ioctl
 *
 * Description:
 *   All ioctl calls will be routed through this method
 *
 ****************************************************************************/

static int k210_uart_ioctl(struct file *filep, int cmd, unsigned long arg)
{
  FAR struct inode *inode    = filep->f_inode;
  FAR struct uart_dev_s *dev = inode->i_private;
  FAR struct k210_uart_s *priv  = (FAR struct k210_uart_s *)dev->priv;
  int ret;

#ifdef CONFIG_SERIAL_UART_ARCH_IOCTL
  ret = uart_ioctl(filep, cmd, arg);

  if (ret != -ENOTTY)
    {
      return ret;
    }

#else
  ret = OK;
#endif

  switch (cmd)
    {
#ifdef CONFIG_SERIAL_TIOCSERGSTRUCT
    case TIOCSERGSTRUCT:
      {
        FAR struct k210_uart_s *user = (FAR struct k210_uart_s *)arg;
        if (!user)
          {
            ret = -EINVAL;
          }
        else
          {
            memcpy(user, dev, sizeof(struct k210_uart_s));
          }
      }
      break;
#endif

    case TIOCSBRK:  /* BSD compatibility: Turn break on, unconditionally */
      {
        irqstate_t flags = enter_critical_section();
        k210_uart_enablebreaks(priv, true);
        leave_critical_section(flags);
      }
      break;

    case TIOCCBRK:  /* BSD compatibility: Turn break off, unconditionally */
      {
        irqstate_t flags;
        flags = enter_critical_section();
        k210_uart_enablebreaks(priv, false);
        leave_critical_section(flags);
      }
      break;

#if defined(CONFIG_SERIAL_TERMIOS) && !defined(CONFIG_K210_UART_SUPRESS_CONFIG)
    case TCGETS:
      {
        FAR struct termios *termiosp = (FAR struct termios *)arg;
        irqstate_t flags;

        if (!termiosp)
          {
            ret = -EINVAL;
            break;
          }

        flags = enter_critical_section();

        cfsetispeed(termiosp, priv->baud);
        termiosp->c_cflag = ((priv->parity != 0) ? PARENB : 0) |
                            ((priv->parity == 1) ? PARODD : 0);
        termiosp->c_cflag |= (priv->stopbits2) ? CSTOPB : 0;
#if defined(CONFIG_SERIAL_IFLOWCONTROL) || defined(CONFIG_SERIAL_OFLOWCONTROL)
        termiosp->c_cflag |= priv->flow ? CRTSCTS : 0;
#endif

        switch (priv->bits)
          {
          case 5:
            termiosp->c_cflag |= CS5;
            break;

          case 6:
            termiosp->c_cflag |= CS6;
            break;

          case 7:
            termiosp->c_cflag |= CS7;
            break;

          case 8:
          default:
            termiosp->c_cflag |= CS8;
            break;
          }

        leave_critical_section(flags);
      }
      break;

    case TCSETS:
      {
        FAR struct termios *termiosp = (FAR struct termios *)arg;
        irqstate_t flags;

        if (!termiosp)
          {
            ret = -EINVAL;
            break;
          }

        flags = enter_critical_section();

        switch (termiosp->c_cflag & CSIZE)
          {
          case CS5:
            priv->bits = 5;
            break;

          case CS6:
            priv->bits = 6;
            break;

          case CS7:
            priv->bits = 7;
            break;

          case CS8:
          default:
            priv->bits = 8;
            break;
          }

        if ((termiosp->c_cflag & PARENB) != 0)
          {
            priv->parity = (termiosp->c_cflag & PARODD) ? 1 : 2;
          }
        else
          {
            priv->parity = 0;
          }

        priv->baud      = cfgetispeed(termiosp);
        priv->stopbits2 = (termiosp->c_cflag & CSTOPB) != 0;
#if defined(CONFIG_SERIAL_IFLOWCONTROL) || defined(CONFIG_SERIAL_OFLOWCONTROL)
        priv->flow      = (termiosp->c_cflag & CRTSCTS) != 0;
#endif

        k210_uart_setup(dev);
        leave_critical_section(flags);
      }
      break;
#endif

    default:
      ret = -ENOTTY;
      break;
    }

  return ret;
}

/****************************************************************************
 * Name: k210_uart_receive
 *
 * Description:
 *   Called (usually) from the interrupt level to receive one
 *   character from the UART.  Error bits associated with the
 *   receipt are provided in the return 'status'.
 *
 ****************************************************************************/

static int k210_uart_receive(struct uart_dev_s *dev, unsigned int *status)
{
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;
  uint32_t rbr = 0;
  *status = k210_uart_serialin(priv, UART_LSR_OFFSET);
  rbr     = k210_uart_serialin(priv, UART_RBR_OFFSET);
  return rbr;
}

/****************************************************************************
 * Name: k210_uart_rxint
 *
 * Description:
 *   Call to enable or disable RX interrupts
 *
 ****************************************************************************/

static void k210_uart_rxint(struct uart_dev_s *dev, bool enable)
{
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;

  if (enable)
    {
      priv->ier |= UART_IER_ERBFI;
    }
  else
    {
      priv->ier &= ~UART_IER_ERBFI;
    }

  k210_uart_serialout(priv, UART_IER_OFFSET, priv->ier);
}

/****************************************************************************
 * Name: k210_uart_rxavailable
 *
 * Description:
 *   Return true if the receive fifo is not empty
 *
 ****************************************************************************/

static bool k210_uart_rxavailable(struct uart_dev_s *dev)
{
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;
  return ((k210_uart_serialin(priv, UART_LSR_OFFSET) & UART_LSR_DR) != 0);
}

/****************************************************************************
 * Name: k210_uart_dma*
 *
 * Description:
 *   Stubbed out DMA-related methods
 *
 ****************************************************************************/

#ifdef CONFIG_SERIAL_IFLOWCONTROL
static bool k210_uart_rxflowcontrol(struct uart_dev_s *dev,
                                 unsigned int nbuffered, bool upper)
{
#ifndef CONFIG_K210_UART_SUPRESS_CONFIG
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;

  if (priv->flow)
    {
      /* Disable Rx interrupt to prevent more data being from
       * peripheral if the RX buffer is near full. When hardware
       * RTS is enabled, this will prevent more data from coming
       * in. Otherwise, enable Rx interrupt to make sure that more
       * input is received.
       */

      k210_uart_rxint(dev, !upper);
      return true;
    }
#endif

  return false;
}
#endif

/****************************************************************************
 * Name: k210_uart_dma*
 *
 * Description:
 *   Stub functions used when serial DMA is enabled.
 *
 ****************************************************************************/

#ifdef CONFIG_SERIAL_TXDMA
static void k210_uart_dmasend(FAR struct uart_dev_s *dev)
{
}
#endif

#ifdef CONFIG_SERIAL_RXDMA
static void k210_uart_dmareceive(FAR struct uart_dev_s *dev)
{
}

static void k210_uart_dmarxfree(FAR struct uart_dev_s *dev)
{
}
#endif

#ifdef CONFIG_SERIAL_TXDMA
static void k210_uart_dmatxavail(FAR struct uart_dev_s *dev)
{
}
#endif

/****************************************************************************
 * Name: k210_uart_send
 *
 * Description:
 *   This method will send one byte on the UART
 *
 ****************************************************************************/

static void k210_uart_send(struct uart_dev_s *dev, int ch)
{
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;
  k210_uart_serialout(priv, UART_THR_OFFSET, (uart_datawidth_t)ch);
}

/****************************************************************************
 * Name: k210_uart_txint
 *
 * Description:
 *   Call to enable or disable TX interrupts
 *
 ****************************************************************************/

static void k210_uart_txint(struct uart_dev_s *dev, bool enable)
{
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;
  irqstate_t flags;

  flags = enter_critical_section();
  if (enable)
    {
      priv->ier |= UART_IER_ETBEI;
      k210_uart_serialout(priv, UART_IER_OFFSET, priv->ier);

      /* Fake a TX interrupt here by just calling uart_xmitchars() with
       * interrupts disabled (note this may recurse).
       */

      uart_xmitchars(dev);
    }
  else
    {
      priv->ier &= ~UART_IER_ETBEI;
      k210_uart_serialout(priv, UART_IER_OFFSET, priv->ier);
    }

  leave_critical_section(flags);
}

/****************************************************************************
 * Name: k210_uart_txready
 *
 * Description:
 *   Return true if the tranmsit fifo is not full
 *
 ****************************************************************************/

static bool k210_uart_txready(struct uart_dev_s *dev)
{
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;
  return (((k210_uart_serialin(priv, UART_LSR_OFFSET) & UART_LSR_THRE) != 0));
}

/****************************************************************************
 * Name: k210_uart_txempty
 *
 * Description:
 *   Return true if the transmit fifo is empty
 *
 ****************************************************************************/

static bool k210_uart_txempty(struct uart_dev_s *dev)
{
  FAR struct k210_uart_s *priv = (FAR struct k210_uart_s *)dev->priv;
  return ((k210_uart_serialin(priv, UART_LSR_OFFSET) & UART_LSR_TEMT) != 0);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
 
/****************************************************************************
 * Name: k210_uart_register
 *
 * Description:
 *   Register serial console and serial ports.  This assumes that
 *   up_earlyserialinit was called previously.
 *
 ****************************************************************************/

void k210_uart_register(void)
{
#if defined(CONFIG_K210_UART1)
    k210_uart_setup(&g_uart1port);
    uart_register("/dev/ttyS1", &g_uart1port);
#endif
#if defined(CONFIG_K210_UART2)
    k210_uart_setup(&g_uart2port);
    uart_register("/dev/ttyS2", &g_uart2port);
#endif
#if defined(CONFIG_K210_UART3)
    k210_uart_setup(&g_uart3port);
    uart_register("/dev/ttyS3", &g_uart3port);
#endif
}

#endif /* CONFIG_K210_UART */
