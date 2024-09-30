/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *******************************************************************************
 * Copyright (C) 2022, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/**
* @file connect_gpio.c
* @brief support edu-arm32-board gpio function using bus driver framework
* @version 3.0 
* @author AIIT XUOS Lab
* @date 2022-12-05
*/

/*************************************************
File name: connect_gpio.c
Description: support edu-arm32-board gpio configure and gpio bus register function
Others: take projects/ev_hc32f4a0_lqfp176/examples/gpio/gpio_output/source/main.c for references
History: 
1. Date: 2022-12-05
Author: AIIT XUOS Lab
Modification: 
1. support edu-arm32-board gpio configure, write and read
2. support edu-arm32-board gpio bus device and driver register
*************************************************/

#include <connect_gpio.h>

#define ITEM_NUM(items)                 sizeof(items) / sizeof(items[0])
#define IRQ_INT(callback)                  

#ifndef HC32_PIN_CONFIG
#define HC32_PIN_CONFIG(pin, callback, config)                                 \
    {                                                                          \
        .pinbit             = pin,                                             \
        .irq_callback       = callback,                                        \
        .irq_config         = config,                                          \
    }
#endif /* HC32_PIN_CONFIG */

#define __HC32_PIN(index, gpio_port, gpio_pin) { 0, GPIO_PORT_##gpio_port, GPIO_PIN_##gpio_pin}
#define __HC32_PIN_DEFAULT                     {-1, 0, 0}
#define MAX_PIN_INDEX 15
#define INT_VECTOR_OFFSET 16
#define INTSEL_REG              (uint32_t)(&CM_INTC->SEL0)

struct PinIndex
{
    int index;
    uint8_t port;
    uint16_t pin;
};

static void EXTINT0_IRQHandler(void);
static void EXTINT1_IRQHandler(void);
static void EXTINT2_IRQHandler(void);
static void EXTINT3_IRQHandler(void);
static void EXTINT4_IRQHandler(void);
static void EXTINT5_IRQHandler(void);
static void EXTINT6_IRQHandler(void);
static void EXTINT7_IRQHandler(void);
static void EXTINT8_IRQHandler(void);
static void EXTINT9_IRQHandler(void);
static void EXTINT10_IRQHandler(void);
static void EXTINT11_IRQHandler(void);
static void EXTINT12_IRQHandler(void);
static void EXTINT13_IRQHandler(void);
static void EXTINT14_IRQHandler(void);
static void EXTINT15_IRQHandler(void);

static struct Hc32PinIrqMap pin_irq_map[] =
{
    HC32_PIN_CONFIG(GPIO_PIN_00, EXTINT0_IRQHandler,  EXTINT0_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_01, EXTINT1_IRQHandler,  EXTINT1_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_02, EXTINT2_IRQHandler,  EXTINT2_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_03, EXTINT3_IRQHandler,  EXTINT3_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_04, EXTINT4_IRQHandler,  EXTINT4_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_05, EXTINT5_IRQHandler,  EXTINT5_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_06, EXTINT6_IRQHandler,  EXTINT6_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_07, EXTINT7_IRQHandler,  EXTINT7_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_08, EXTINT8_IRQHandler,  EXTINT8_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_09, EXTINT9_IRQHandler,  EXTINT9_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_10, EXTINT10_IRQHandler, EXTINT10_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_11, EXTINT11_IRQHandler, EXTINT11_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_12, EXTINT12_IRQHandler, EXTINT12_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_13, EXTINT13_IRQHandler, EXTINT13_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_14, EXTINT14_IRQHandler, EXTINT14_IRQ_CONFIG),
    HC32_PIN_CONFIG(GPIO_PIN_15, EXTINT15_IRQHandler, EXTINT15_IRQ_CONFIG),
};

static const struct PinIndex pins[] = 
{
    __HC32_PIN_DEFAULT,
    __HC32_PIN(1, E, 02),
    __HC32_PIN(2, E, 03),
    __HC32_PIN(3, E, 04),
    __HC32_PIN(4, E, 05),
    __HC32_PIN(5, E, 06),
    __HC32_PIN_DEFAULT,
    __HC32_PIN(7, I, 08),
    __HC32_PIN_DEFAULT,
    __HC32_PIN(9, C, 14),
    __HC32_PIN(10, C, 15),
    __HC32_PIN(11, I, 09),
    __HC32_PIN(12, I, 10),
    __HC32_PIN(13, I, 11),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(16, F, 00),
    __HC32_PIN(17, F, 01),
    __HC32_PIN(18, F, 02),
    __HC32_PIN(19, F, 03),
    __HC32_PIN(20, F, 04),
    __HC32_PIN(21, F, 05),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(24, F, 06),
    __HC32_PIN(25, F, 07),
    __HC32_PIN(26, F, 08),
    __HC32_PIN(27, F, 09),
    __HC32_PIN(28, F, 10),
    __HC32_PIN(29, H, 00),
    __HC32_PIN(30, H, 01),
    __HC32_PIN_DEFAULT,
    __HC32_PIN(32, C, 00),
    __HC32_PIN(33, C, 01),
    __HC32_PIN(34, C, 02),
    __HC32_PIN(35, C, 03),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(40, A, 00),
    __HC32_PIN(41, A, 01),
    __HC32_PIN(42, A, 02),
    __HC32_PIN(43, H, 02),
    __HC32_PIN(44, H, 03),
    __HC32_PIN(45, H, 04),
    __HC32_PIN(46, H, 05),
    __HC32_PIN(47, A, 03),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(50, A, 04),
    __HC32_PIN(51, A, 05),
    __HC32_PIN(52, A, 06),
    __HC32_PIN(53, A, 07),
    __HC32_PIN(54, C, 04),
    __HC32_PIN(55, C, 05),
    __HC32_PIN(56, B, 00),
    __HC32_PIN(57, B, 01),
    __HC32_PIN(58, B, 02),
    __HC32_PIN(59, F, 11),
    __HC32_PIN(60, F, 12),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(63, F, 13),
    __HC32_PIN(64, F, 14),
    __HC32_PIN(65, F, 15),
    __HC32_PIN(66, G, 00),
    __HC32_PIN(67, G, 01),
    __HC32_PIN(68, E, 07),
    __HC32_PIN(69, E, 08),
    __HC32_PIN(70, E, 09),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(73, E, 10),
    __HC32_PIN(74, E, 11),
    __HC32_PIN(75, E, 12),
    __HC32_PIN(76, E, 13),
    __HC32_PIN(77, E, 14),
    __HC32_PIN(78, E, 15),
    __HC32_PIN(79, B, 10),
    __HC32_PIN(80, B, 11),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(83, H, 06),
    __HC32_PIN(84, H, 07),
    __HC32_PIN(85, H, 08),
    __HC32_PIN(86, H, 09),
    __HC32_PIN(87, H, 10),
    __HC32_PIN(88, H, 11),
    __HC32_PIN(89, H, 12),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(92, B, 12),
    __HC32_PIN(93, B, 13),
    __HC32_PIN(94, B, 14),
    __HC32_PIN(95, B, 15),
    __HC32_PIN(96, D, 08),
    __HC32_PIN(97, D, 09),
    __HC32_PIN(98, D, 10),
    __HC32_PIN(99, D, 11),
    __HC32_PIN(100, D, 12),
    __HC32_PIN(101, D, 13),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(104, D, 14),
    __HC32_PIN(105, D, 15),
    __HC32_PIN(106, G, 02),
    __HC32_PIN(107, G, 03),
    __HC32_PIN(108, G, 04),
    __HC32_PIN(109, G, 05),
    __HC32_PIN(110, G, 06),
    __HC32_PIN(111, G, 07),
    __HC32_PIN(112, G, 08),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(115, C, 06),
    __HC32_PIN(116, C, 07),
    __HC32_PIN(117, C, 08),
    __HC32_PIN(118, C, 09),
    __HC32_PIN(119, A, 08),
    __HC32_PIN(120, A, 09),
    __HC32_PIN(121, A, 10),
    __HC32_PIN(122, A, 11),
    __HC32_PIN(123, A, 12),
    __HC32_PIN(124, A, 13),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(128, H, 13),
    __HC32_PIN(129, H, 14),
    __HC32_PIN(130, H, 15),
    __HC32_PIN(131, I, 00),
    __HC32_PIN(132, I, 01),
    __HC32_PIN(133, I, 02),
    __HC32_PIN(134, I, 03),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(137, A, 14),
    __HC32_PIN(138, A, 15),
    __HC32_PIN(139, C, 10),
    __HC32_PIN(140, C, 11),
    __HC32_PIN(141, C, 12),
    __HC32_PIN(142, D, 00),
    __HC32_PIN(143, D, 01),
    __HC32_PIN(144, D, 02),
    __HC32_PIN(145, D, 03),
    __HC32_PIN(146, D, 04),
    __HC32_PIN(147, D, 05),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(150, D, 06),
    __HC32_PIN(151, D, 07),
    __HC32_PIN(152, G, 09),
    __HC32_PIN(153, G, 10),
    __HC32_PIN(154, G, 11),
    __HC32_PIN(155, G, 12),
    __HC32_PIN(156, G, 13),
    __HC32_PIN(157, G, 14),
    __HC32_PIN_DEFAULT,
    __HC32_PIN_DEFAULT,
    __HC32_PIN(160, G, 15),
    __HC32_PIN(161, B, 03),
    __HC32_PIN(162, B, 04),
    __HC32_PIN(163, B, 05),
    __HC32_PIN(164, B, 06),
    __HC32_PIN(165, B, 07),
    __HC32_PIN(166, I, 13),
    __HC32_PIN(167, B, 08),
    __HC32_PIN(168, B, 09),
    __HC32_PIN(169, E, 00),
    __HC32_PIN(170, E, 01),
    __HC32_PIN(171, I, 12),
    __HC32_PIN_DEFAULT,
    __HC32_PIN(173, I, 04),
    __HC32_PIN(174, I, 05),
    __HC32_PIN(175, I, 06),
    __HC32_PIN(176, I, 07),
};

struct PinIrqHdr pin_irq_hdr_tab[] =
{
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE},
    {-1, 0, NONE, NONE}
};

static int GpioPinIndex(uint16_t pin){
    int ret = 0;
    for(;ret<=MAX_PIN_INDEX;ret++){ //ret must be 16-bit 
        if((0x0001U<<ret)&pin){
            KPrintf("the int pin is %d\n",ret);
            return ret;
        }
    };
    return -1;
}

static void PinIrqHandler(uint16_t pinbit)
{
    int32_t irqindex = -1;

    if (SET == EXTINT_GetExtIntStatus(pinbit))
    {
        EXTINT_ClearExtIntStatus(pinbit);
        irqindex = __CLZ(__RBIT(pinbit));
        if (pin_irq_hdr_tab[irqindex].hdr) {
            pin_irq_hdr_tab[irqindex].hdr(pin_irq_hdr_tab[irqindex].args);
        }
    }
}

static void EXTINT0_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[0].pinbit);
}

static void EXTINT1_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[1].pinbit);
}

static void EXTINT2_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[2].pinbit);
}

static void EXTINT3_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[3].pinbit);
}

static void EXTINT4_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[4].pinbit);
}

static void EXTINT5_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[5].pinbit);
}

static void EXTINT6_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[6].pinbit);
}

static void EXTINT7_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[7].pinbit);
}

static void EXTINT8_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[8].pinbit);
}

static void EXTINT9_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[9].pinbit);
}

static void EXTINT10_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[10].pinbit);
}

static void EXTINT11_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[11].pinbit);
}

static void EXTINT12_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[12].pinbit);
}

static void EXTINT13_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[13].pinbit);
}

static void EXTINT14_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[14].pinbit);
}

static void EXTINT15_IRQHandler(void)
{
    PinIrqHandler(pin_irq_map[15].pinbit);
}

const struct PinIndex *GetPin(uint8_t pin)
{
    const struct PinIndex *index;

    if (pin < ITEM_NUM(pins)) {
        index = &pins[pin];
        if (index->index == -1)
            index = NONE;
    } else {
        index = NONE;
    }

    return index;
}

static int32 GpioConfigMode(int mode, const struct PinIndex* index)
{
    stc_gpio_init_t stcGpioInit;
    NULL_PARAM_CHECK(index);

    GPIO_StructInit(&stcGpioInit);

    switch (mode)
    {
    case GPIO_CFG_OUTPUT:
        stcGpioInit.u16PinDir        = PIN_DIR_OUT;
        stcGpioInit.u16PinOutputType = PIN_OUT_TYPE_CMOS;
        break;
    case GPIO_CFG_INPUT:
        stcGpioInit.u16PinDir   = PIN_DIR_IN;
        stcGpioInit.u16ExtInt = PIN_EXTINT_ON;
        break;
    case GPIO_CFG_INPUT_PULLUP:
        stcGpioInit.u16PinDir   = PIN_DIR_IN;
        stcGpioInit.u16PullUp   = PIN_PU_ON;
        break;
    case GPIO_CFG_INPUT_PULLDOWN:
        stcGpioInit.u16PinDir   = PIN_DIR_IN;
        stcGpioInit.u16PullUp   = PIN_PU_OFF;
        break;
    case GPIO_CFG_OUTPUT_OD:
        stcGpioInit.u16PinDir        = PIN_DIR_OUT;
        stcGpioInit.u16PinOutputType = PIN_OUT_TYPE_NMOS;
        break;
    default:
        break;
    }
    GPIO_Init(index->port, index->pin, &stcGpioInit);
}

static int32 GpioIrqRegister(int32 pin, int32 mode, void (*hdr)(void *args), void *args)
{
    const struct PinIndex *index = GetPin(pin);
    int32 irqindex = -1;

    stc_extint_init_t stcExtIntInit;

    irqindex = GpioPinIndex(index->pin); // start from 0
    if (irqindex >= ITEM_NUM(pin_irq_map)) {
        return -ENONESYS;
    }

    x_base level = CriticalAreaLock();
    if (pin_irq_hdr_tab[irqindex].pin == pin   &&
        pin_irq_hdr_tab[irqindex].hdr == hdr   &&
        pin_irq_hdr_tab[irqindex].mode == mode &&
        pin_irq_hdr_tab[irqindex].args == args
    ) {
        CriticalAreaUnLock(level);
        return EOK;
    }
    if (pin_irq_hdr_tab[irqindex].pin != -1) {
        CriticalAreaUnLock(level);
        return -EDEV_BUSY;
    }
    pin_irq_hdr_tab[irqindex].pin = pin;
    pin_irq_hdr_tab[irqindex].hdr = hdr;
    pin_irq_hdr_tab[irqindex].mode = mode;
    pin_irq_hdr_tab[irqindex].args = args;

    /* Extint config */
    EXTINT_StructInit(&stcExtIntInit);
    switch (mode)
    {
    case GPIO_IRQ_EDGE_RISING:
        stcExtIntInit.u32Edge = EXTINT_TRIG_RISING;
        break;
    case GPIO_IRQ_EDGE_FALLING:
        stcExtIntInit.u32Edge = EXTINT_TRIG_FALLING;
        break;
    case GPIO_IRQ_EDGE_BOTH:
        stcExtIntInit.u32Edge = EXTINT_TRIG_BOTH;
        break;
    case GPIO_IRQ_LEVEL_LOW:
        stcExtIntInit.u32Edge = EXTINT_TRIG_LOW;
        break;
    }
    EXTINT_Init(index->pin, &stcExtIntInit);   

    __IO uint32_t *INTC_SELx = (__IO uint32_t *)(INTSEL_REG + (4U * (uint32_t)(irqindex)));
    WRITE_REG32(*INTC_SELx, irqindex);
    isrManager.done->registerIrq(irqindex+INT_VECTOR_OFFSET, (void(*)(int vector,void *))hdr, args);

    CriticalAreaUnLock(level);
    return EOK;
}

static uint32 GpioIrqFree(x_base pin)
{
    const struct PinIndex* index = GetPin(pin);
    int32 irqindex = -1;

    irqindex = GpioPinIndex(index->pin);
    if (irqindex >= ITEM_NUM(pin_irq_map)) {
        return -ENONESYS;
    }

    x_base level = CriticalAreaLock();
    if (pin_irq_hdr_tab[irqindex].pin == -1) {
        CriticalAreaUnLock(level);
        return EOK;
    }
    isrManager.done->freeIrq(pin_irq_hdr_tab[irqindex].pin);
    pin_irq_hdr_tab[irqindex].pin  = -1;
    pin_irq_hdr_tab[irqindex].hdr  = NONE;
    pin_irq_hdr_tab[irqindex].mode = 0;
    pin_irq_hdr_tab[irqindex].args = NONE;
    CriticalAreaUnLock(level);
 
    return EOK;
}

static void GpioIrqConfig(uint8_t u8Port, uint16_t u16Pin, uint16_t u16ExInt)
{
    __IO uint16_t *PCRx;
    uint16_t pin_num;

    pin_num = __CLZ(__RBIT(u16Pin));
    PCRx = (__IO uint16_t *)((uint32_t)(&CM_GPIO->PCRA0) + ((uint32_t)u8Port * 0x40UL) + (pin_num * 4UL));
    MODIFY_REG16(*PCRx, GPIO_PCR_INTE, u16ExInt);
}

static int32 GpioIrqEnable(x_base pin)
{
    struct Hc32PinIrqMap *irq_map;
    const struct PinIndex* index = GetPin(pin);
    int32 irqindex = -1;

    irqindex = GpioPinIndex(index->pin);
    if (irqindex >= ITEM_NUM(pin_irq_map)) {
        return -ENONESYS;
    }

    x_base level = CriticalAreaLock();
    if (pin_irq_hdr_tab[irqindex].pin == -1) {
        CriticalAreaUnLock(level);
        return -ENONESYS;
    }

    GpioIrqConfig(index->port, index->pin, PIN_EXTINT_ON);
    isrManager.done->enableIrq(GpioPinIndex(index->pin));

    CriticalAreaUnLock(level);
    return EOK;
}

static int32 GpioIrqDisable(x_base pin)
{
    struct Hc32PinIrqMap *irq_map;
    const struct PinIndex* index = GetPin(pin);

    x_base level = CriticalAreaLock();

    GpioIrqConfig(index->port, index->pin, PIN_EXTINT_OFF);
    isrManager.done->disableIrq(GpioPinIndex(index->pin));

    CriticalAreaUnLock(level);
    return EOK;
}

static uint32 Hc32PinConfigure(struct PinParam *param)
{
    NULL_PARAM_CHECK(param);    
    uint32 ret = EOK;

    const struct PinIndex *index = GetPin(param->pin);
    switch(param->cmd)
    {
        case GPIO_CONFIG_MODE:
            GpioConfigMode(param->mode, index);
            break;
        case GPIO_IRQ_REGISTER:
            ret = GpioIrqRegister(param->pin, param->irq_set.irq_mode, param->irq_set.hdr, param->irq_set.args);
            break;
        case GPIO_IRQ_FREE:
            ret = GpioIrqFree(param->pin);
            break;
        case GPIO_IRQ_ENABLE:
            ret = GpioIrqEnable(param->pin);
            break;
        case GPIO_IRQ_DISABLE:
            ret = GpioIrqDisable(param->pin);
            break;
        default:
            ret = -EINVALED;
            break;
    }

    return ret;
}

static uint32 Hc32PinInit(void)
{
    static x_bool pin_init_flag = RET_FALSE;

    if (!pin_init_flag) {
        pin_init_flag = RET_TRUE;
    }
    
    return EOK;
}

static uint32 Hc32GpioDrvConfigure(void *drv, struct BusConfigureInfo *configure_info)
{
    NULL_PARAM_CHECK(drv);
    NULL_PARAM_CHECK(configure_info);

    x_err_t ret = EOK;
    struct PinParam *param;

    switch (configure_info->configure_cmd)
    {
        case OPE_INT:
            ret = Hc32PinInit();
            break;    
        case OPE_CFG:
            param = (struct PinParam *)configure_info->private_data;
            ret = Hc32PinConfigure(param);
            break;
        default:
            break;
    }

    return ret;
}

uint32 Hc32PinWrite(void *dev, struct BusBlockWriteParam *write_param)
{
    NULL_PARAM_CHECK(dev);
    NULL_PARAM_CHECK(write_param);

    struct PinStat *pinstat = (struct PinStat *)write_param->buffer;
    const struct PinIndex* index = GetPin(pinstat->pin);
    NULL_PARAM_CHECK(index);

    if (GPIO_LOW == pinstat->val) {
        GPIO_ResetPins(index->port, index->pin);
    } else {
        GPIO_SetPins(index->port, index->pin);
    }

    return EOK;
}

uint32 Hc32PinRead(void *dev, struct BusBlockReadParam *read_param)
{
    NULL_PARAM_CHECK(dev);
    NULL_PARAM_CHECK(read_param);
    struct PinStat *pinstat = (struct PinStat *)read_param->buffer;
    const struct PinIndex* index = GetPin(pinstat->pin);
    NULL_PARAM_CHECK(index);
    
    if(GPIO_ReadInputPins(index->port, index->pin) == PIN_RESET) {
        pinstat->val = GPIO_LOW;
    } else {
        pinstat->val = GPIO_HIGH;
    }
    return pinstat->val;
}

static const struct PinDevDone dev_done =
{
    .open  = NONE,
    .close = NONE,
    .write = Hc32PinWrite,
    .read  = Hc32PinRead,
};

int HwGpioInit(void)
{
    x_err_t ret = EOK;

    static struct PinBus pin;
    memset(&pin, 0, sizeof(struct PinBus));

    ret = PinBusInit(&pin, PIN_BUS_NAME);
    if (ret != EOK) {
        KPrintf("gpio bus init error %d\n", ret);
        return ERROR;
    }

    static struct PinDriver drv;
    memset(&drv, 0, sizeof(struct PinDriver));
    drv.configure = &Hc32GpioDrvConfigure;
    
    ret = PinDriverInit(&drv, PIN_DRIVER_NAME, NONE);
    if (ret != EOK) {
        KPrintf("pin driver init error %d\n", ret);
        return ERROR;
    }

    ret = PinDriverAttachToBus(PIN_DRIVER_NAME, PIN_BUS_NAME);
    if (ret != EOK) {
        KPrintf("pin driver attach error %d\n", ret);
        return ERROR;
    }

    static struct PinHardwareDevice dev;
    memset(&dev, 0, sizeof(struct PinHardwareDevice));
    dev.dev_done = &dev_done;

    ret = PinDeviceRegister(&dev, NONE, PIN_DEVICE_NAME);
    if (ret != EOK) {
        KPrintf("pin device register error %d\n", ret);
        return ERROR;
    }

    ret = PinDeviceAttachToBus(PIN_DEVICE_NAME, PIN_BUS_NAME);
    if (ret != EOK) {
        KPrintf("pin device register error %d\n", ret);
        return ERROR;
    }

    return ret;
}

//#define GPIO_LED_TEST
#ifdef GPIO_LED_TEST

static void GpioLedDelay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 8000000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

void GpioLedTest(void)
{
    BusType pin;
    struct BusConfigureInfo configure_info;
    struct BusBlockWriteParam write_param;

    int ret = 0;
    int pinSet = -1;

    pin = BusFind(PIN_BUS_NAME);
    if (!pin) {
        KPrintf("find %s failed!\n", PIN_BUS_NAME);
        return;
    }
    pin->owner_driver = BusFindDriver(pin, PIN_DRIVER_NAME);
    pin->owner_haldev = BusFindDevice(pin, PIN_DEVICE_NAME);

    configure_info.configure_cmd = OPE_INT;
    ret = BusDrvConfigure(pin->owner_driver, &configure_info);
    if (ret != EOK) {
        KPrintf("initialize %s failed!\n", PIN_BUS_NAME);
        return;
    }

    struct PinParam led_gpio_param;
    struct PinStat led_gpio_stat;

    // 134 -> PortI Pin03 LED3
    x_base pinIndex = 134;
    led_gpio_param.cmd = GPIO_CONFIG_MODE;
    led_gpio_param.pin = pinIndex;
    led_gpio_param.mode = GPIO_CFG_OUTPUT;

    configure_info.configure_cmd = OPE_CFG;
    configure_info.private_data = (void *)&led_gpio_param;
    ret = BusDrvConfigure(pin->owner_driver, &configure_info);
    if (ret != EOK) {
        KPrintf("config pin failed!\n");
        return;
    }

    while (1) {
        GpioLedDelay();

        if (pinSet) {
            /* set led pin as high*/
            led_gpio_stat.pin = pinIndex;
            led_gpio_stat.val = GPIO_HIGH;
            write_param.buffer = (void *)&led_gpio_stat;
            BusDevWriteData(pin->owner_haldev, &write_param);
            pinSet = 0;
        } else {
            /* set led pin as low*/
            led_gpio_stat.pin = pinIndex;
            led_gpio_stat.val = GPIO_LOW;
            write_param.buffer = (void *)&led_gpio_stat;
            BusDevWriteData(pin->owner_haldev, &write_param);
            pinSet = 1;
        }
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                                                GpioLedTest, GpioLedTest, gpio led test);
#endif
