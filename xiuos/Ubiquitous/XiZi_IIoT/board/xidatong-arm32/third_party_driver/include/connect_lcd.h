/**
* @file connect_lcd.h
* @brief define xidatong-arm32 lcd function
* @version 1.0 
* @author AIIT XiUOS Lab
* @date 2022-04-25
*/

#ifndef CONNECT_LCD_H
#define CONNECT_LCD_H

#include <device.h>

#ifdef BSP_USING_TOUCH
#include "connect_touch.h"
#endif


int Imxrt1052HwLcdInit(void);
#endif
