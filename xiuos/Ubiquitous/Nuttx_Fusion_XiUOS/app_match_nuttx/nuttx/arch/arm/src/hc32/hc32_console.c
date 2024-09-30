
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
#include <fcntl.h>
#include <debug.h>

#include <arch/board/board.h>
#include "chip.h"
#include "hc32_uart.h"
#include "hc32_spi.h"

#define CMP_STR(_tar, _str) (strncmp(_tar, _str, strlen(_str)) == 0)
extern int hc32_i2c_test(void);

void hc32_test_console(void)
{
    char *dev_str = "/dev/console";
    char *test_chr = "test";
    int fd = 0, ret;

    fd = open(dev_str, 0x6);
    hc32_print("%s: open <%s> ret = %d\n", __func__, dev_str, fd);
    ret = write(fd, test_chr, strlen(test_chr));
    hc32_print("%s: open %d ret %d\n", __func__, fd, ret);
    close(fd);
}

void hc32_bmp180_test(void)
{
    char *bmp_dev = "/dev/bmp180";
    char write_arr[] = {0xF4, 0x2E};
    int fd = 0, ret;

    fd = open(bmp_dev, 0x6);
    ret = write(fd, write_arr, 2);
    hc32_print("%s: write attr ret %x\n", __func__, ret);
    close(fd);
}

void hc32_console_handle(char *buf)
{
    if(CMP_STR(buf, "console"))
    {
        hc32_test_console();
    }
    else if(CMP_STR(buf, "spi"))
    {
        hc32_print("start flash test %d ...\n", g_system_timer);
        hc32_spiflash_test();
    }
    else if(CMP_STR(buf, "i2c"))
    {
        hc32_print("start i2c test %d ...\n", g_system_timer);
        hc32_i2c_test();
    }
    else if(CMP_STR(buf, "bmp"))
    {
        hc32_print("start bmp180 test ...\n");
        hc32_bmp180_test();
    }
    else if(CMP_STR(buf, "pr"))
    {
        printf("z\n");
        hc32_print("start pr test %d ...\n", g_system_timer);
        printf("b\n");
    }
}

