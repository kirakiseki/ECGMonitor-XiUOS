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
 * @file stm32_hs300x.c
 * @brief stm32 hs300x
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.04.12
 */


/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdio.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/i2c/i2c_master.h>
#include <nuttx/sensors/hs300x.h>

#include "stm32.h"
#include "stm32_i2c.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_hs300x_initialize
 *
 * Description:
 *   Initialize and register the hs300x Temperature Sensor driver.
 *
 * Input Parameters:
 *   devno - The device number
 *   busno - The I2C bus number
 *   used to build the device path as /dev/i2c1_dev0
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int board_hs300x_initialize(int devno, int busno)
{
  FAR struct i2c_master_s *i2c;
  char devpath[20];
  int ret;

  sninfo("Initializing HS300x!\n");

  /* Get an instance of the I2C interface */
  i2c =  stm32_i2cbus_initialize(busno);
  if (!i2c)
    {
      return -ENODEV;
    }

  /* Then register the temperature sensor,such as /dev/i2c1_dev0. */
  snprintf(devpath, 20, "/dev/i2c%d_dev%d", busno, devno);
  ret = hs300x_register(devpath, i2c, CONFIG_SENSOR_DEVICE_HS300X_I2C_ADDR);
  if (ret < 0)
    {
      stm32_i2cbus_uninitialize(i2c);
    }

  return ret;
}
