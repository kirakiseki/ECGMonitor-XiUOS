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
* @file connect_uart.h
* @brief define edu-arm32-board usart function and struct
* @version 2.0
* @author AIIT XUOS Lab
* @date 2023-02-09
*/

#include <device.h>
#include <hardware_irq.h>
#include <hc32_ll_adc.h>


#ifdef __cplusplus
extern "C" {
#endif

struct HwAdc 
{
    CM_ADC_TypeDef *ADCx;
    uint8 adc_channel;
};

int HwAdcInit(void);

#ifdef __cplusplus
}
#endif


