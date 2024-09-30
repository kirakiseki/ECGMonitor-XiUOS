/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v10x_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/04/30
* Description        : Main Interrupt Service Routines.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
/*************************************************
File name: ch32v30x_it.c
Description: include peripheral supports for ch32v30x 
History: 
1. Date: 2022-08-09
Author: AIIT XUOS Lab
Modification:
1. add HardFault interrupt implementation.
*************************************************/
#include "ch32v30x_it.h"
#include "board.h"
#include <xs_isr.h>



void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
    GET_INT_SP();
    isrManager.done->incCounter();
    KPrintf("NMI_Handler.\n");
    isrManager.done->decCounter();
    FREE_INT_SP();
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
    GET_INT_SP();
    isrManager.done->incCounter();
    KPrintf("HardFault_Handler.\n");
    isrManager.done->decCounter();
    FREE_INT_SP();
}



