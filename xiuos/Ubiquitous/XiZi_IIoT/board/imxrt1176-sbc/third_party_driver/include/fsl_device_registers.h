/*
 * Copyright 2014-2016 Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/**
* @file fsl_device_registers.h
* @brief 
* @version 1.0 
* @author AIIT XUOS Lab
* @date 2022-08-19
*/

/*************************************************
File name: fsl_device_registers.h
Description: 
Others: take SDK_2_11_1_MIMXRT1170-EVK for references
               
History: 
1. Date: 2022-08-19
Author: AIIT XUOS Lab
Modification: 
*************************************************/

#ifndef __FSL_DEVICE_REGISTERS_H__
#define __FSL_DEVICE_REGISTERS_H__

/*
 * Include the cpu specific register header files.
 *
 * The CPU macro should be declared in the project or makefile.
 */
#if (defined(CPU_MIMXRT1176AVM8A_cm7) || defined(CPU_MIMXRT1176CVM8A_cm7) || defined(CPU_MIMXRT1176DVMAA_cm7))

#define MIMXRT1176_cm7_SERIES

/* CMSIS-style register definitions */
#include "MIMXRT1176_cm7.h"
/* CPU specific feature definitions */
#include "MIMXRT1176_cm7_features.h"

#elif (defined(CPU_MIMXRT1176AVM8A_cm4) || defined(CPU_MIMXRT1176CVM8A_cm4) || defined(CPU_MIMXRT1176DVMAA_cm4))

#define MIMXRT1176_cm4_SERIES

/* CMSIS-style register definitions */
#include "MIMXRT1176_cm4.h"
/* CPU specific feature definitions */
#include "MIMXRT1176_cm4_features.h"

#else
    #error "No valid CPU defined!"
#endif

#endif /* __FSL_DEVICE_REGISTERS_H__ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
