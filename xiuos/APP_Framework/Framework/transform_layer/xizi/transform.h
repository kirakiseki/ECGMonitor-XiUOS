/*
* Copyright (c) 2020 AIIT XUOS Lab
* XiUOS is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*        http://license.coscl.org.cn/MulanPSL2
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
*/

/**
 * @file transform.h
 * @brief Interface function declarations required by the framework
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2021.06.04
 */

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <timer.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <user_api.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OPE_INT                 0x0000
#define OPE_CFG                 0x0001

#define OPER_WDT_SET_TIMEOUT    0x0002
#define OPER_WDT_KEEPALIVE      0x0003

#define NAME_NUM_MAX            32

#ifndef EVENT_AND
#define EVENT_AND          (1 << 0)
#endif
#ifndef EVENT_OR
#define EVENT_OR           (1 << 1)
#endif
#ifndef EVENT_AUTOCLEAN
#define EVENT_AUTOCLEAN    (1 << 2)
#endif

/*********************GPIO define*********************/
#define GPIO_LOW    0x00
#define GPIO_HIGH   0x01

#define GPIO_CFG_OUTPUT             0x00
#define GPIO_CFG_INPUT              0x01
#define GPIO_CFG_INPUT_PULLUP       0x02
#define GPIO_CFG_INPUT_PULLDOWN     0x03
#define GPIO_CFG_OUTPUT_OD          0x04

#define GPIO_CONFIG_MODE            0xffffffff

/********************SERIAL define*******************/
#define BAUD_RATE_2400          2400
#define BAUD_RATE_4800          4800
#define BAUD_RATE_9600          9600
#define BAUD_RATE_19200         19200
#define BAUD_RATE_38400         38400
#define BAUD_RATE_57600         57600
#define BAUD_RATE_115200        115200
#define BAUD_RATE_230400        230400
#define BAUD_RATE_460800        460800
#define BAUD_RATE_921600        921600
#define BAUD_RATE_2000000       2000000
#define BAUD_RATE_3000000       3000000

#define DATA_BITS_5             5
#define DATA_BITS_6             6
#define DATA_BITS_7             7
#define DATA_BITS_8             8
#define DATA_BITS_9             9

#define STOP_BITS_1             1
#define STOP_BITS_2             2
#define STOP_BITS_3             3
#define STOP_BITS_4             4

#define PARITY_NONE             1
#define PARITY_ODD              2
#define PARITY_EVEN             3

#define BIT_ORDER_LSB           1
#define BIT_ORDER_MSB           2

#define NRZ_NORMAL              1
#define NRZ_INVERTED            2

#ifndef SERIAL_RB_BUFSZ
#define SERIAL_RB_BUFSZ         128
#endif

/********************SPI define*******************/
#define SPI_MAX_CLOCK            40000000
#define spi_device_max_num       4

#define SPI_LINE_CPHA            (1 << 0)                           
#define SPI_LINE_CPOL            (1 << 1)                          

#define SPI_LSB                  (0 << 2)                             
#define SPI_MSB                  (1 << 2)                             

#define SPI_DEV_MASTER           (0 << 3)                            
#define SPI_DEV_SLAVE            (1 << 3)      

#define SPI_MODE_0               (0 | 0)                        
#define SPI_MODE_1               (0 | SPI_LINE_CPHA)              
#define SPI_MODE_2               (SPI_LINE_CPOL | 0)            
#define SPI_MODE_3               (SPI_LINE_CPOL | SPI_LINE_CPHA)    
#define SPI_MODE_MASK            (SPI_LINE_CPHA | SPI_LINE_CPOL | SPI_MSB)

#define SPI_CS_HIGH              (1 << 4)                            
#define SPI_NO_CS                (1 << 5)                           
#define SPI_3WIRE                (1 << 6)                             
#define SPI_READY                (1 << 7)

struct PinDevIrq
{
    int irq_mode;//< RISING/FALLING/HIGH/LOW
    void (*hdr) (void *args);//< callback function
    void *args;//< the params of callback function
};

struct PinParam
{
    int cmd;//< cmd:GPIO_CONFIG_MODE/GPIO_IRQ_REGISTER/GPIO_IRQ_FREE/GPIO_IRQ_DISABLE/GPIO_IRQ_ENABLE
    long  pin;//< pin number
    int mode;//< pin mode: input/output
    struct PinDevIrq irq_set;//< pin irq set
    uint64_t arg;
};

struct PinStat
{
    long pin;//< pin number
    uint16_t val;//< pin level
};

enum ExtSerialPortConfigure
{
    PORT_CFG_INIT = 0,
    PORT_CFG_PARITY_CHECK,
    PORT_CFG_DISABLE,
    PORT_CFG_DIV,
};

struct SerialDataCfg
{
    uint32_t serial_baud_rate;
    uint8_t serial_data_bits;
    uint8_t serial_stop_bits;
    uint8_t serial_parity_mode;
    uint8_t serial_bit_order;
    uint8_t serial_invert_mode;
    uint16_t serial_buffer_size;
    int32 serial_timeout;

    uint8_t is_ext_uart;
    uint8_t ext_uart_no;
    enum ExtSerialPortConfigure port_configure;
};

struct SpiMasterParam
{
    uint8 spi_work_mode;//CPOL CPHA
    uint8 spi_frame_format;//frame format
    uint8 spi_data_bit_width;//bit width
    uint8 spi_data_endian;//little endian：0，big endian：1
    uint32 spi_maxfrequency;//work frequency
};

enum IoctlDriverType
{
    SERIAL_TYPE = 0,
    SPI_TYPE,
    I2C_TYPE,
    PIN_TYPE,
    LCD_TYPE,
    ADC_TYPE,
    DAC_TYPE,
    WDT_TYPE,
    RTC_TYPE,
    CAMERA_TYPE,
    CAN_TYPE,
    KPU_TYPE,
    FLASH_TYPE,
    TIME_TYPE,
    DEFAULT_TYPE,
};


struct DvpRegConfigureInfo
{
    uint8_t device_addr;
    uint16_t reg_addr;
    uint8_t reg_value;
} ;

struct PrivIoctlCfg
{
    enum IoctlDriverType ioctl_driver_type;
    void *args;
};

typedef struct 
{
    uint16 x_pos;
    uint16 y_pos;
    uint16 width;
    uint16 height;
    uint8  font_size;
    uint8 *addr;
    uint16 font_color;
    uint16 back_color;
}LcdStringParam;

typedef struct 
{
    uint16 x_startpos;
    uint16 x_endpos;
    uint16 y_startpos;
    uint16 y_endpos;
    void* pixel_color;
}LcdPixelParam;

struct CameraCfg
{
    uint16_t window_w;
    uint16_t window_h;
    uint16_t window_xoffset;
    uint16_t window_yoffset;
    uint16_t output_w;
    uint16_t output_h;
    uint8_t gain;
    uint8_t gain_manu_enable;
};

typedef struct 
{
    char type; // 0:write string;1:write dot
    LcdPixelParam pixel_info;
    LcdStringParam string_info;
}LcdWriteParam;

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t press;
}TouchDataParam;

struct TouchDataStandard
{
    uint16 x;
    uint16 y;
};

struct RtcDrvConfigureParam
{
    int rtc_operation_cmd;
    time_t *time;
};

typedef struct 
{
    uintptr_t pdata; 
    uint32_t length;
}_ioctl_shoot_para;

typedef struct 
{
    uint32_t width;         // width   The width  of image
    uint32_t height;        // height  The height of image
}_ioctl_set_reso;

typedef struct 
{
    uintptr_t r_addr;
    uintptr_t g_addr;
    uintptr_t b_addr;
}RgbAddress;

enum TCP_OPTION {
    SEND_DATA = 0,
    RECV_DATA,
};

struct CanDriverConfigure 
{
    uint8 tsjw;
    uint8 tbs2 ;
    uint8 tbs1;
    uint8 mode;
    uint16 brp;
};

struct CanSendConfigure
{
    uint32 stdid;
    uint32 exdid;
    uint8 ide;
    uint8 rtr;
    uint8 data_lenth;
    uint8 *data;
};

typedef struct
{
    uint8_t *buffer;
    size_t length;
}KpuOutputBuffer;

#define PRIV_SYSTICK_GET (CurrentTicksGain())
#define PRIV_LCD_DEV "/dev/lcd_dev"
#define MY_DISP_HOR_RES BSP_LCD_Y_MAX
#define MY_DISP_VER_RES BSP_LCD_X_MAX

#define PRIV_TOUCH_DEV "/dev/touch_dev"
#define MY_INDEV_X BSP_LCD_Y_MAX
#define MY_INDEV_Y BSP_LCD_X_MAX

#define LCD_STRING_TYPE 0
#define LCD_DOT_TYPE 1
#define LCD_SIZE 320
#define IMAGE_HEIGHT 240
#define IMAGE_WIDTH 320
#define NULL_PARAMETER 0

#define REG_SCCB_READ 0xA2U
#define REG_SCCB_WRITE 0xA3U
#define SCCB_REG_LENGTH 0x08U

#define SET_DISPLAY_ADDR (0xD1)
#define SET_AI_ADDR (0xD2)
#define FLAG_CHECK (0xD4)

#define LOAD_KMODEL 0xA0
#define RUN_KMODEL 0xA1
#define GET_OUTPUT 0xA2
#define WAIT_FLAG 0xA3

#define IOCTRL_CAMERA_START_SHOT            (22)     // start shoot
#define IOCTRL_CAMERA_OUT_SIZE_RESO (23)
#define IOCTRL_CAMERA_SET_WINDOWS_SIZE      (21)     // user set specific windows outsize
#define IOCTRL_CAMERA_SET_LIGHT             (24)     //set light mode
#define IOCTRL_CAMERA_SET_COLOR             (25)     //set color saturation
#define IOCTRL_CAMERA_SET_BRIGHTNESS        (26)     //set color brightness
#define IOCTRL_CAMERA_SET_CONTRAST          (27)     //set contrast
#define IOCTRL_CAMERA_SET_EFFECT            (28)     //set effect
#define IOCTRL_CAMERA_SET_EXPOSURE          (29)     //set auto exposure
/*********************shell***********************/

#ifndef SEPARATE_COMPILE
//for int func(int argc, char *agrv[])
#define PRIV_SHELL_CMD_MAIN_ATTR (SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN))

//for int func(int i, char ch, char *str)
#define PRIV_SHELL_CMD_FUNC_ATTR (SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC))

/**
 * @brief Priv-shell Command definition 
 * 
 * @param _func Command function 
 * @param _desc Command description 
 * @param _attr Command attributes if need
 */
#define PRIV_SHELL_CMD_FUNCTION(_func, _desc, _attr)  \
    SHELL_EXPORT_CMD(_attr, _func, _func, _desc)

#else
//for int func(int argc, char *agrv[])
#define PRIV_SHELL_CMD_MAIN_ATTR() 

//for int func(int i, char ch, char *str)
#define PRIV_SHELL_CMD_FUNC_ATTR()
#define PRIV_SHELL_CMD_FUNCTION(_func, _desc, _attr) 

#endif
/**********************mutex**************************/

int PrivMutexCreate(pthread_mutex_t *p_mutex, const pthread_mutexattr_t *attr);
int PrivMutexDelete(pthread_mutex_t *p_mutex);
int PrivMutexObtain(pthread_mutex_t *p_mutex);
int PrivMutexAbandon(pthread_mutex_t *p_mutex);

/*********************semaphore**********************/

int PrivSemaphoreCreate(sem_t *sem, int pshared, unsigned int value);
int PrivSemaphoreDelete(sem_t *sem);
int PrivSemaphoreObtainWait(sem_t *sem, const struct timespec *abstime);
int PrivSemaphoreObtainNoWait(sem_t *sem);
int PrivSemaphoreAbandon(sem_t *sem);
int32_t PrivSemaphoreSetValue(int32_t sem, uint16_t val);

/*********************event**********************/
#ifndef SEPARATE_COMPILE
int PrivEventCreate(uint8_t flag);
int PrivEvenDelete(int event);
int PrivEvenTrigger(int event, uint32_t set);
int PrivEventProcess(int event, uint32_t set, uint8_t option, int32_t wait_time, unsigned int *Recved);
#endif

/*********************task**************************/

int PrivTaskCreate(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);
int PrivTaskStartup(pthread_t *thread);
int PrivTaskDelete(pthread_t thread, int sig);
void PrivTaskQuit(void *value_ptr);
int PrivTaskDelay(int32_t ms);
int PrivUserTaskSearch(void);
uint32_t PrivGetTickTime();

/*********************driver*************************/

int PrivOpen(const char *path, int flags);
int PrivRead(int fd, void *buf, size_t len);
int PrivWrite(int fd, const void *buf, size_t len);
int PrivClose(int fd);
int PrivIoctl(int fd, int cmd, void *args);

/*********************memory***********************/

void *PrivMalloc(size_t size);
void *PrivRealloc(void *pointer, size_t size);
void *PrivCalloc(size_t  count, size_t size);
void PrivFree(void *pointer);

/******************soft timer*********************/
int PrivTimerCreate(clockid_t clockid, struct sigevent * evp, timer_t * timerid);
int PrivTimerDelete(timer_t timerid);
int PrivTimerStartRun(timer_t timerid);
int PrivTimerQuitRun(timer_t timerid);
int PrivTimerModify(timer_t timerid, int flags, const struct itimerspec *restrict value,
                  struct itimerspec *restrict ovalue);

#ifdef __cplusplus
}
#endif

#endif