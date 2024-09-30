/*
File Name    : type.h
Author       : Yichip
Version      : V1.0
Date         : 2018/05/25
Description  : Data type definition.
*/

#ifndef __TYPE_H__
#define __TYPE_H__

#if defined (__CC_ARM) || defined ( __ICCARM__ )
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char      byte;
typedef unsigned short     word;

typedef signed char       int8_t;
typedef signed short      int16_t;
typedef signed int        int32_t;
typedef signed long long  int64_t;
#else
#include "stdio.h"
typedef unsigned char      byte;
typedef unsigned short     word;
#endif



#endif /*__TYPE_H__*/

