
/**
* @file connect_touch.c
* @brief support xidatong touch function and register to bus framework
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2022-04-25
*/

#ifndef CONNECT_TOUCH_H
#define CONNECT_TOUCH_H

#include <device.h>	 

/* 表示读数据 */ 
#define I2C_M_RD		0x0001

struct i2c_msg {
	uint8_t addr;		/*从设备的I2C设备地址 */
	uint16_t flags;	    /*控制标志 */
	uint16_t len;		/*读写数据的长度	*/
	uint8_t *buf;		/*存储读写数据的指针 */
};

typedef struct 
{
  int  X;
  int  Y;
}POINT;

typedef enum _touch_event
{
    kTouch_Down = 0,    /*!< The state changed to touched. */
    kTouch_Up = 1,      /*!< The state changed to not touched. */
    kTouch_Contact = 2, /*!< There is a continuous touch being detected. */
    kTouch_Reserved = 3 /*!< No touch information available. */
} touch_event_t;

/*设定使用的电容屏IIC设备地址*/
#define GTP_ADDRESS            0xBA

#define GTP_MAX_HEIGHT   272
#define GTP_MAX_WIDTH    480
#define GTP_INT_TRIGGER  0
#define GTP_MAX_TOUCH    5

#define GTP_CONFIG_MAX_LENGTH 240
#define GTP_ADDR_LENGTH       2

// Registers define
#define GTP_READ_COOR_ADDR    0x814E
#define GTP_REG_SLEEP         0x8040
#define GTP_REG_SENSOR_ID     0x814A
#define GTP_REG_CONFIG_DATA   0x8047
#define GTP_REG_VERSION       0x8140

#define CFG_GROUP_LEN(p_cfg_grp)  (sizeof(p_cfg_grp) / sizeof(p_cfg_grp[0]))

int HwTouchInit(void);

#endif
