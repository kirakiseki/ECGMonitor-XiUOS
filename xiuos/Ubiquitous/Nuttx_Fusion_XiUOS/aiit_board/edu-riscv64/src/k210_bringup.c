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
 * @file k210_bringup.c
 * @brief edu-riscv64 k210_bringup.c
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.03.17
 */


/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdbool.h>
#include <stdio.h>
#include <debug.h>
#include <errno.h>

#include <nuttx/board.h>
#include <nuttx/fs/fs.h>
#include <nuttx/arch.h>

#include "k210.h"
#include "k210_clockconfig.h"
#include "edu-riscv64.h"
#include <arch/board/board.h>
#include "k210_sysctl.h"
#include "k210_fpioa.h"
#include "k210_gpiohs.h"
#include "k210_gpio_common.h"

#ifdef CONFIG_BSP_USING_TOUCH
#  include "k210_touch.h"
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: k210_bringup
 ****************************************************************************/

int k210_bringup(void)
{
  int ret = OK;

#ifdef CONFIG_FS_PROCFS
  /* Mount the procfs file system */

  ret = nx_mount(NULL, "/proc", "procfs", 0, NULL);
  if (ret < 0)
    {
      serr("ERROR: Failed to mount procfs at %s: %d\n", "/proc", ret);
    }
#endif

#ifdef CONFIG_DEV_GPIO
  ret = k210_gpio_init();
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize GPIO Driver: %d\n", ret);
      return ret;
    }
#endif

#ifdef CONFIG_K210_UART1
  sysctl_clock_enable(SYSCTL_CLOCK_UART1);
  sysctl_reset(SYSCTL_RESET_UART1);

  fpioa_set_function(GPIO_WIFI_TXD, FPOA_USART1_RX);
  fpioa_set_function(GPIO_WIFI_RXD, FPOA_USART1_TX);

  fpioa_set_function(GPIO_WIFI_EN, K210_IO_FUNC_GPIOHS0 + FPIOA_WIFI_EN);
  k210_gpiohs_set_direction(FPIOA_WIFI_EN, GPIO_DM_OUTPUT);
  k210_gpiohs_set_value(FPIOA_WIFI_EN, GPIO_PV_LOW);
  up_mdelay(50);
  k210_gpiohs_set_value(FPIOA_WIFI_EN, GPIO_PV_HIGH);
#endif

#ifdef CONFIG_K210_UART2
  sysctl_clock_enable(SYSCTL_CLOCK_UART2);
  sysctl_reset(SYSCTL_RESET_UART2);

  fpioa_set_function(GPIO_E220_RXD, FPOA_USART2_RX);
  fpioa_set_function(GPIO_E220_TXD, FPOA_USART2_TX);
#endif

#ifdef CONFIG_K210_UART3
  sysctl_clock_enable(SYSCTL_CLOCK_UART3);
  sysctl_reset(SYSCTL_RESET_UART3);
  
  fpioa_set_function(GPIO_CH376T_RXD, FPOA_USART3_RX);
  fpioa_set_function(GPIO_CH376T_TXD, FPOA_USART3_TX);
#endif

  return ret;
}
