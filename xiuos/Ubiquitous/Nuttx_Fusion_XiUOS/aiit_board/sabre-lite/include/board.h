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
 * @file board.h
 * @brief sabre-lite board.h file
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2023.05.15
 */

#ifndef __BOARDS_ARM_IMX6_SABRE_LITE_INCLUDE_BOARD_H
#define __BOARDS_ARM_IMX6_SABRE_LITE_INCLUDE_BOARD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Clocking *****************************************************************/

/* The Sabre-Lite board has two crystals:
 *
 *   Y1    24     MHz CPU_XTALI/CPU_XTALO
 *   QZ500 32.768 KHz RTC_XTALI/RTC_XTALO
 */

#define BOARD_CPUXTAL_FREQUENCY 24000000
#define BAORD_RTCXTAL_FREQUENCY    32768

/* Clocking will be configured at 792 MHz initially when started via U-Boot.
 * The Linux kernel will use the CPU frequency scaling code which will switch
 * the processor frequency between 400 MHz and 1GHz based on load and
 * temperature.
 *
 * These are the frequencies reported with U-Boot starts up:
 *
 *   mx6q pll1      : 792MHz
 *   mx6q pll2      : 528MHz
 *   mx6q pll3      : 480MHz
 *   mx6q pll8      :  50MHz
 *
 *   ipg clock      :  66000000Hz
 *   ipg per clock  :  66000000Hz
 *   uart clock     :  80000000Hz
 *   cspi clock     :  60000000Hz
 *   ahb clock      : 132000000Hz
 *   axi clock      : 264000000Hz
 *   emi_slow clock :  29333333Hz
 *   ddr clock      : 528000000Hz
 *   usdhc1 clock   : 198000000Hz
 *   usdhc2 clock   : 198000000Hz
 *   usdhc3 clock   : 198000000Hz
 *   usdhc4 clock   : 198000000Hz
 *   nfc clock      :  24000000Hz
 *
 * For now, NuttX simply leaves the clocking at 792MHz.
 */

/* LED definitions **********************************************************/

/* LEDs
 *
 * A single LED is available driven GPIO1_IO02.
 * On the schematic this is USR_DEF_RED_LED signal to pin T1 (GPIO_2).
 * This signal is shared with KEY_ROW6 (ALT2).
 * A high value illuminates the LED.
 */

/* LED index values for use with board_userled() */

#define BOARD_LED           0
#define BOARD_NLEDS         1

/* LED bits for use with board_userled_all() */

#define BOARD_LED_BIT       (1 << BOARD_LED)

/* These LEDs are not used by the board port unless CONFIG_ARCH_LEDS is
 * defined.  In that case, the usage by the board port is defined in
 * include/board.h and src/sam_autoleds.c. The LEDs are used to encode
 * OS-related events as follows:
 *
 *   ---------------------- ---------------------------- ------
 *   SYMBOL                     Meaning                  LED
 *   ---------------------- ---------------------------- ------
 */

#define LED_STARTED         0 /* NuttX has been started  OFF      */
#define LED_HEAPALLOCATE    0 /* Heap has been allocated OFF      */
#define LED_IRQSENABLED     0 /* Interrupts enabled      OFF      */
#define LED_STACKCREATED    1 /* Idle stack created      ON       */
#define LED_INIRQ           2 /* In an interrupt         N/C      */
#define LED_SIGNAL          2 /* In a signal handler     N/C      */
#define LED_ASSERTION       2 /* An assertion failed     N/C      */
#define LED_PANIC           3 /* The system has crashed  FLASH    */
#undef  LED_IDLE              /* MCU is is sleep mode    Not used */

/* Thus is LED is statically on, NuttX has successfully  booted and is,
 * apparently, running normally.  If LED is flashing at approximately
 * 2Hz, then a fatal error has been detected and the system has halted.
 */

/* Button definitions *******************************************************/

/* GPIO Disambiguation ******************************************************/

/* A DEBUG VCOM is available MICRO USB AB 5 J509.
 * This corresponds to UART1 from the i.MX6.
 * UART1 connects to J509 via the CSIO_DAT10 and CSIO_DAT11 pins:
 */

#define GPIO_UART1_RX_DATA  GPIO_UART1_RX_DATA_2
#define GPIO_UART1_TX_DATA  GPIO_UART1_TX_DATA_2

#define GPIO_UART2_RX_DATA  GPIO_UART2_RX_DATA_2
#define GPIO_UART2_TX_DATA  GPIO_UART2_TX_DATA_2

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
 * Public Functions Definitions
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_ARM_IMX6_SABRE_LITE_INCLUDE_BOARD_H */
