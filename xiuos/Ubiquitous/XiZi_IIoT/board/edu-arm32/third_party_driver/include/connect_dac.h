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
#include <hc32_ll_fcg.h>
#include <hc32_ll_dac.h>
#include <hc32_ll_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct HwDac 
{
    CM_DAC_TypeDef *DACx;
    uint16 digital_data;
};

typedef struct {
    CM_DAC_TypeDef *pUnit;
    // en_dac_cvt_t enCvtType;
    uint16_t u16Ch;
} stc_dac_handle_t;


int HwDacInit(void);

#ifdef __cplusplus
}
#endif


