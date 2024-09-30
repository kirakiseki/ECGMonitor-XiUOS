#ifndef __HC32_CONSOLE_H_
#define __HC32_CONSOLE_H_

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/fs/ioctl.h>
#include <nuttx/serial/serial.h>
#include <nuttx/power/pm.h>

#ifdef CONFIG_SERIAL_TERMIOS
#  include <termios.h>
#endif

#include <arch/board/board.h>
#include "chip.h"
#include "hc32_uart.h"
#include "hc32_rcc.h"
#include "hc32_gpio.h"
#include "arm_internal.h"
#include "hc32f4a0.h"
#include "hc32f4a0_usart.h"
#include "hc32f4a0_gpio.h"
#include "hc32f4a0_interrupts.h"
#include "hc32f4a0_sram.h"
#include "hc32f4a0_pwc.h"
#include "hc32f4a0_efm.h"

void hc32_console_handle(char *buf);

#endif
