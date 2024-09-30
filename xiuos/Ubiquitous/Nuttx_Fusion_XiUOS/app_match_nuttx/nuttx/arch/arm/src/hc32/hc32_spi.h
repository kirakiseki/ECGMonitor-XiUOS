/**
 *******************************************************************************
 * @file  arch/arm/src/hc32/hc32_spi.h
 * @brief SPI write read flash API for the Device Driver Library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2020-06-12       Wangmin         First version
   2020-10-13       Wangmin         Modify spelling mistake
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
#include "hc32_ddl.h"

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/* SPI unit and clock definition */
#define SPI_UNIT                        (M4_SPI1)
#define SPI_UNIT_CLOCK                  (PWC_FCG1_SPI1)

/* SPI port definition for master */
#define SPI_NSS_PORT                    (GPIO_PORT_C)
#define SPI_NSS_PIN                     (GPIO_PIN_07)

#define SPI_SCK_PORT                    (GPIO_PORT_C)
#define SPI_SCK_PIN                     (GPIO_PIN_06)
#define SPI_SCK_FUNC                    (GPIO_FUNC_40_SPI1_SCK)

#define SPI_MOSI_PORT                   (GPIO_PORT_D)
#define SPI_MOSI_PIN                    (GPIO_PIN_08)
#define SPI_MOSI_FUNC                   (GPIO_FUNC_41_SPI1_MOSI)

#define SPI_MISO_PORT                   (GPIO_PORT_D)
#define SPI_MISO_PIN                    (GPIO_PIN_09)
#define SPI_MISO_FUNC                   (GPIO_FUNC_42_SPI1_MISO)

/* NSS pin control */
#define SPI_NSS_HIGH()                  (GPIO_SetPins(SPI_NSS_PORT, SPI_NSS_PIN))
#define SPI_NSS_LOW()                   (GPIO_ResetPins(SPI_NSS_PORT, SPI_NSS_PIN))

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

int hc32_spi_init(void);
void hc32_spiflash_test(void);

