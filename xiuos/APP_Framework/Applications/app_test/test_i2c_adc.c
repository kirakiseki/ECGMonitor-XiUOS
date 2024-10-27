#include <stdio.h>
#include <string.h>
#include "sysctl.h"
#include <transform.h>

#ifdef ADD_XIZI_FEATURES

#define I2C_SLAVE_ADDRESS 0x48

//Define 1-shot read or continuous
#define ONESHOT_ON  0b10000001
#define ONESHOT_OFF 0

//Define Analog Inputs
#define AI0 0b01000000
#define AI1 0b01010000
#define AI2 0b01100000
#define AI3 0b01110000

//Define gain setting
#define GAIN1 0b00000000  //Full Scale Voltage is: 6.144 + or -
#define GAIN2 0b00000010  //FS 4.096
#define GAIN3 0b00000100  //FS 2.048
#define GAIN4 0b00000110  //FS 1.024
#define GAIN5 0b00001000  //FS  .512
#define GAIN6 0b00001010  //FS  .256
#define GAIN7 0b00001100  //FS  .256
#define GAIN8 0b00001110  //FS  .256

#define SAMPLES_COUNT 5000

static char config[3] = {
    0x01, 
    AI1 + GAIN2,
    0xE3
};

uint16_t dat[SAMPLES_COUNT];

void TestI2CADC(void)
{
    // config I2C pin(SCL:34.SDA:35) in menuconfig
    int iic_fd = PrivOpen(I2C_DEV_DRIVER, O_RDWR);
    if (iic_fd < 0)
    {
        printf("open iic_fd fd error:%d\n", iic_fd);
        return;
    }
    printf("IIC open successful!\n");

    // init I2C
    uint16 iic_address = I2C_SLAVE_ADDRESS;

    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = I2C_TYPE;
    ioctl_cfg.args = (void *)&iic_address;

    if (0 != PrivIoctl(iic_fd, OPE_INT, &ioctl_cfg))
    {
        printf("ioctl iic fd error %d\n", iic_fd);
        PrivClose(iic_fd);
        return;
    }
    printf("IIC configure successful!\n");

    // I2C read and write
    char readBuf[2];
    PrivWrite(iic_fd, config, 3);
    memset(readBuf, 0, sizeof(readBuf));
    memset(dat, 0, sizeof(dat));
    PrivTaskDelay(8);
    PrivWrite(iic_fd, readBuf, 1);
    int cnt = 0;

    // Start time
    uint64_t start = read_cycle();

    // Measure for SAMPLES_COUNT samples
    while (cnt<SAMPLES_COUNT)
    {
        PrivRead(iic_fd, readBuf, sizeof(readBuf));
        uint16_t result = readBuf[0] << 8 | readBuf[1];
        dat[cnt++] = result;
    }

    // Elapsed time
    uint64_t elapsed = read_cycle() - start;
    double seconds = (double)elapsed/(sysctl_cpu_get_freq());

    // Output sample data
    for(int i = 0; i < SAMPLES_COUNT; i++)
    {
        printf("%d,", dat[i]);
    }
    printf("\n");

    // Output statistics
    printf("Direct I2C:\n");
    printf("Time used: %f\n", seconds);
    printf("SPS: %f\n", SAMPLES_COUNT / seconds);

    // Close I2C fd
    PrivClose(iic_fd);
    return;
}

extern void ECGADC_ADS1115();

PRIV_SHELL_CMD_FUNCTION(TestI2CADC, a iic adc test sample, PRIV_SHELL_CMD_MAIN_ATTR);
PRIV_SHELL_CMD_FUNCTION(ECGADC_ADS1115, a iic adc test sample, PRIV_SHELL_CMD_MAIN_ATTR);
#endif