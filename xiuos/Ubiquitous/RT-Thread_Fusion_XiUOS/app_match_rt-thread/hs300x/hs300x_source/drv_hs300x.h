#ifndef __HS300X_H__
#define __HS300X_H__

#include <rtthread.h>
#include <rtdevice.h>

#include <rthw.h>

#ifdef __cplusplus
extern "C"{
#endif
#ifndef HS300X_I2C_FREQUENCY
#define HS300X_I2C_FREQUENCY  40000
#endif

#define I2C_NAME "i2c1"
#define buflen 4
#define HS300X_TEMP_RANGE_MAX (80)
#define HS300X_TEMP_RANGE_MIN (-10)






#ifdef __cplusplus
}
#endif
#endif