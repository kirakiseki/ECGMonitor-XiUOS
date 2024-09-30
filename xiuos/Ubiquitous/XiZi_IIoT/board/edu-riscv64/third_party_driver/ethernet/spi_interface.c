
#include <assert.h>
#include <connect_w5500.h>
#include <drv_io_config.h>
#include <fpioa.h>
#include <sleep.h>
#include <xs_base.h>

#include "gpio_common.h"
#include "gpiohs.h"

// #define SPI1_CS_GPIONUM 24

static x_base g_w5500_spi_lock;
/**
 * @brief  进入临界区
 * @retval None
 */
void spi_enter_cris(void) { g_w5500_spi_lock = DisableLocalInterrupt(); }
/**
 * @brief  退出临界区
 * @retval None
 */
void spi_exit_cris(void) { EnableLocalInterrupt(g_w5500_spi_lock); }

/**
 * @brief  片选信号输出低电平
 * @retval None
 */
void spi_select_cs(void) { gpiohs_set_pin(SPI1_CS0_PIN, GPIO_PV_LOW); }
/**
 * @brief  片选信号输出高电平
 * @retval None
 */
void spi_deselete_cs(void) { gpiohs_set_pin(SPI1_CS0_PIN, GPIO_PV_HIGH); }