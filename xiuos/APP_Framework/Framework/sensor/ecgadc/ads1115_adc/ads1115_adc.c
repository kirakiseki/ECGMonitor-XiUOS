#include <sensor.h>

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

static char config[3] = {
    0x01, 
    AI1 + GAIN2,
    0xE3
};

static char readBuf[2] = {0x00, 0x00};

static struct SensorDevice ads1115;

static struct SensorProductInfo info =
{
    (SENSOR_ABILITY_ADC),
    "TI",
    "ADS1115",
};

/**
 * @description: Open ADS1115 sensor device
 * @param sdev - sensor device pointer
 * @return success : 0 error : -1
 */
static int SensorDeviceOpen(struct SensorDevice *sdev)
{
    int result;
    uint16_t i2c_dev_addr = SENSOR_DEVICE_ADS1115_I2C_ADDR;
    
    sdev->fd = PrivOpen(SENSOR_DEVICE_ADS1115_DEV, O_RDWR);
    if (sdev->fd < 0) {
        printf("open %s error\n", SENSOR_DEVICE_ADS1115_DEV);
        return -1;
    }

    struct PrivIoctlCfg ioctl_cfg;
    ioctl_cfg.ioctl_driver_type = I2C_TYPE;
    ioctl_cfg.args = &i2c_dev_addr;
    result = PrivIoctl(sdev->fd, OPE_INT, &ioctl_cfg);

    PrivWrite(sdev->fd, config, 3);
    PrivTaskDelay(8);
    PrivWrite(sdev->fd, readBuf, 1);

    return result;
}

/**
 * @description: Write sensor device
 * @param sdev - sensor device pointer
 * @param len - the length of the read data
 * @return success: 0 , failure: -1
 */
static int SensorDeviceWrite(struct SensorDevice *sdev, const void *buf, size_t len)
{
    if (PrivWrite(sdev->fd, buf, len) < 0)
        return -1;

    return 0;
}

/**
 * @description: Read sensor device
 * @param sdev - sensor device pointer
 * @param len - the length of the read data
 * @return success: 0 , failure: -1
 */
static int SensorDeviceRead(struct SensorDevice *sdev, size_t len)
{
    //Read i2c device data from i2c device address
    if (PrivRead(sdev->fd, sdev->buffer, len) < 0)
        return -1;

    return 0;
}

static struct SensorDone done =
{
    SensorDeviceOpen,
    NULL,
    SensorDeviceRead,
    SensorDeviceWrite,
    NULL,
};



/**
 * @description: Init ADS1115 sensor and register
 * @return void
 */
static void SensorDeviceADS1115Init(void)
{
    ads1115.name = SENSOR_DEVICE_ADS1115;
    ads1115.info = &info;
    ads1115.done = &done;
    ads1115.status = SENSOR_DEVICE_PASSIVE;

    SensorDeviceRegister(&ads1115);
}


static struct SensorQuantity ads1115_ecgadc;

/**
 * @description: Analysis ADS1115 ECGADC result
 * @param quant - sensor quantity pointer
 * @return quantity value
 */
static int32_t ReadECGADC(struct SensorQuantity *quant)
{
    if (!quant)
        return -1;

    float result;
    if (quant->sdev->done->read != NULL) {
        if (quant->sdev->status == SENSOR_DEVICE_PASSIVE) {
            quant->sdev->done->read(quant->sdev, 2);
            
            return quant->sdev->buffer[0] << 8 | quant->sdev->buffer[1];

        }
        if (quant->sdev->status == SENSOR_DEVICE_ACTIVE) {
            printf("Please set passive mode.\n");
        }
    }else{
        printf("%s don't have read done.\n", quant->name);
    }
    
    return -1;
}

/**
 * @description: Init ADS1115 ECGADC quantity and register
 * @return 0
 */
int ADS1115ECGADCInit(void)
{
    SensorDeviceADS1115Init();
    
    ads1115_ecgadc.name = SENSOR_QUANTITY_ADS1115_ECGADC;
    ads1115_ecgadc.type = SENSOR_QUANTITY_ADC;
    ads1115_ecgadc.value.decimal_places = 0;
    ads1115_ecgadc.value.max_std = (1<<16);
    ads1115_ecgadc.value.min_std = 0;
    ads1115_ecgadc.value.last_value = SENSOR_QUANTITY_VALUE_ERROR;
    ads1115_ecgadc.value.max_value = SENSOR_QUANTITY_VALUE_ERROR;
    ads1115_ecgadc.value.min_value = SENSOR_QUANTITY_VALUE_ERROR;
    ads1115_ecgadc.sdev = &ads1115;
    ads1115_ecgadc.ReadValue = ReadECGADC;

    SensorQuantityRegister(&ads1115_ecgadc);

    return 0;
}