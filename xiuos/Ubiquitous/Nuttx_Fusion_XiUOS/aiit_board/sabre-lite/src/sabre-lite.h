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
 * @file sabre-lite.h
 * @brief sabre-lite sabre-lite.h file
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2023.05.15
 */

#ifndef __BOARDS_ARM_IMX6_SABRE_LITE_SRC_SABRE_LITE_H
#define __BOARDS_ARM_IMX6_SABRE_LITE_SRC_SABRE_LITE_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include "imx_gpio.h"
#include "imx_iomuxc.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Configuration ************************************************************/

/* Sabre-Lite GPIO Pin Definitions *****************************************/

/* LED
 *
 * A single LED is available driven GPIO1_IO02.
 * On the schematic this is USR_DEF_RED_LED signal to pin T1 (GPIO_2).
 * This signal is shared with KEY_ROW6 (ALT2).
 * A high value illuminates the LED.
 */

#define IOMUX_LED  (IOMUX_PULL_NONE | IOMUX_CMOS_OUTPUT | \
                    IOMUX_DRIVE_40OHM | \
                    IOMUX_SPEED_MEDIUM | IOMUX_SLEW_SLOW)
#define GPIO_LED   (GPIO_OUTPUT | GPIO_OUTPUT_ZERO | \
                    GPIO_PORT1 | GPIO_PIN2 | \
                    IOMUX_LED)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Public Functions Definitions
 ****************************************************************************/

/****************************************************************************
 * Name: imx_bringup
 *
 * Description:
 *   Bring up board features
 *
 ****************************************************************************/

#if defined(CONFIG_BOARDCTL) || defined(CONFIG_BOARD_LATE_INITIALIZE)
int imx_bringup(void);
#endif

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_ARM_SABRE_LITE_SRC_SABRE_LITE_H */
