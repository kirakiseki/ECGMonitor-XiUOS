/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-29     tyustli      first version
 *
 */

#include <rtdevice.h>
#include <rtthread.h>
#include "drv_gpio.h"
#include <board.h>
#include "fsl_gpio.h"
#include "board/hardware/ch438/ch438.h"
/* defined the LED pin: GPIO1_IO9 */
#define LED0_PIN               GET_PIN(1,9)
extern int FrameworkInit(void);
int main(void)
{   
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_kprintf("XIUOS xidatong build %s %s\n",__DATE__,__TIME__);
    FrameworkInit();
	  while (1)
    {    
        rt_thread_mdelay(5000);
     }
}

#ifdef BSP_USING_SDRAM
static void sdram_test2(void)
{
    char *p =NULL;
	p = rt_malloc(1024*1024*8);
	if(p == NULL)
	{
		rt_kprintf("apply for 8MB memory fail ~!!!");
	}
	else
	{
		rt_kprintf("appyle for 8MB memory success!!!");
	}
	rt_free(p);
}
MSH_CMD_EXPORT(sdram_test2, sdram test2);
#endif

#ifdef BSP_USB1_HOST
#include <dfs_posix.h>
static char test_data[120], buffer[120];
#define TEST_FN     "/testusb.c"
void readwrite(const char* filename)
{
    int fd;
    int index, length;

    fd = open(TEST_FN, O_WRONLY | O_CREAT | O_TRUNC, 0);
    if (fd < 0)
    {
        rt_kprintf("open file for write failed\n");
        return;
    }

    for (index = 0; index < sizeof(test_data); index ++)
    {
        test_data[index] = index + 27;
    }

    length = write(fd, test_data, sizeof(test_data));
    if (length != sizeof(test_data))
    {
        rt_kprintf("write data failed\n");
        close(fd);
        return;
    }

    close(fd);

    fd = open(TEST_FN, O_RDONLY, 0);
    if (fd < 0)
    {
        rt_kprintf("check: open file for read failed\n");
        return;
    }

    length = read(fd, buffer, sizeof(buffer));
    if (length != sizeof(buffer))
    {
        rt_kprintf("check: read file failed\n");
        close(fd);
        return;
    }

    for (index = 0; index < sizeof(test_data); index ++)
    {
        if (test_data[index] != buffer[index])
        {
            rt_kprintf("check: check data failed at %d\n", index);
            close(fd);
            return;
        }
    }

    rt_kprintf("usb host read/write udisk successful\r\n");

    close(fd);
}

MSH_CMD_EXPORT(readwrite, usb host read write test);

#endif