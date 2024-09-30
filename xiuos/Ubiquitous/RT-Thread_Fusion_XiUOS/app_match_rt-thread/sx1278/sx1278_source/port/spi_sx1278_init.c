#include <rtthread.h>

#include "drv_spi.h"
#include <board.h>
#include <spi_msd.h>
#include <dfs_elm.h>
#include <dfs_fs.h>
#include <drv_spi.h>
#include <dfs_posix.h>
#include <rtdbg.h>

static int sx12xx_spi_device_init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();
    rt_hw_spi_device_attach("spi2","spi20",GPIOC,GPIO_PIN_6);
}
INIT_APP_EXPORT(sx12xx_spi_device_init);