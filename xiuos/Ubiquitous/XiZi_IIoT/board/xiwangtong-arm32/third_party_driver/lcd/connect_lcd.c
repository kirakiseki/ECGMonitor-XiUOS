/*
* Copyright (c) 2022 AIIT XUOS Lab
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
* @file connect_lcd.c
* @brief support imxrt1052 lcd function and register to bus framework
* @version 2.0 
* @author AIIT XiUOS Lab
* @date 2022-04-25
*/

#include <connect_lcd.h>
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_elcdif.h"
#include "fsl_gpio.h"

/* Back light. */
#define LCD_BL_GPIO     GPIO2
#define LCD_HSW 20
#define LCD_HFP 100
#define LCD_HBP 26
#define LCD_VSW 3
#define LCD_VFP 22
#define LCD_VBP 23

#define LCD_HEIGHT  BSP_LCD_X_MAX
#define LCD_WIDTH   BSP_LCD_Y_MAX


static uint16_t frame_buffer[LCD_HEIGHT][LCD_WIDTH] SECTION("NonCacheable.init");
static void InitLcdifPixelClock(void)
{
    /*
     * The desired output frame rate is 60Hz. So the pixel clock frequency is:
     * (480 + 41 + 4 + 18) * (272 + 10 + 4 + 2) * 60 = 9.2M.
     * Here set the LCDIF pixel clock to 9.3M.
     */

    /*
     * Initialize the Video PLL.
     * Video PLL output clock is OSC24M * (loopDivider + (denominator / numerator)) / postDivider = 93MHz.
     */
    clock_video_pll_config_t pll_config;

    pll_config.loopDivider = 31;
    pll_config.postDivider = 4;
    pll_config.numerator   = 0;
    pll_config.denominator = 0;

    CLOCK_InitVideoPll(&pll_config);
        /*
     * 000 derive clock from PLL2
     * 001 derive clock from PLL3 PFD3
     * 010 derive clock from PLL5
     * 011 derive clock from PLL2 PFD0
     * 100 derive clock from PLL2 PFD1
     * 101 derive clock from PLL3 PFD1
     */
    CLOCK_SetMux(kCLOCK_LcdifPreMux, 2);

    CLOCK_SetDiv(kCLOCK_LcdifPreDiv, 2);

    CLOCK_SetDiv(kCLOCK_LcdifDiv, 1);
}

static void InitLcdBacklight(uint8_t level)
{
    gpio_pin_config_t config = {
        kGPIO_DigitalOutput,
        0,
    };

    /* Backlight. */
    config.outputLogic = level;
    GPIO_PinInit(LCD_BL_GPIO, LCD_BL_GPIO_PIN, &config);
}

static void ELCDFramebuffSet(void)
{
    /* LCD */
    elcdif_rgb_mode_config_t lcd_config;

    lcd_config.panelWidth    = LCD_WIDTH;
    lcd_config.panelHeight   = LCD_HEIGHT;
    lcd_config.hsw           = LCD_HSW;
    lcd_config.hfp           = LCD_HFP;
    lcd_config.hbp           = LCD_HBP;
    lcd_config.vsw           = LCD_VSW;
    lcd_config.vfp           = LCD_VFP;
    lcd_config.vbp           = LCD_VBP;

    lcd_config.polarityFlags = kELCDIF_DataEnableActiveHigh |
                               kELCDIF_VsyncActiveLow       |
                               kELCDIF_HsyncActiveLow       |
                               kELCDIF_DriveDataOnRisingClkEdge;

    lcd_config.bufferAddr    = (uint32_t)frame_buffer;
    lcd_config.pixelFormat   = kELCDIF_PixelFormatRGB565;
    lcd_config.dataBus       = kELCDIF_DataBus16Bit; 

    ELCDIF_RgbModeInit (LCDIF, &lcd_config);
}

static void HwLcdInit()
{
    memset(frame_buffer, 0, sizeof(frame_buffer));

    /*step1： config PLL clock */
    InitLcdifPixelClock();

    /*step2: config backlight gpio*/
    InitLcdBacklight(GPIO_HIGH);

    /*step3: fill framebuff*/
    ELCDFramebuffSet();

    ELCDIF_RgbModeStart(LCDIF);

}
static void DrvLcdSetPixelDot(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, void* color)
{
    uint16_t i = 0;
    uint16_t j = 0;

    for(i = y1; i <= y2; i++) {
        for(j = x1; j <= x2; j++) {
            frame_buffer[i][j] =(*(uint16_t*)color);
            color += sizeof(uint16_t);
        }
    }
}

static uint32 LcdWrite(void *dev, struct BusBlockWriteParam *write_param)
{
    x_err_t ret = EOK;

    if (write_param->buffer) 
    {
        LcdWriteParam *show = (LcdWriteParam *)write_param->buffer;
        //output string
        if(0 == show->type) {
            
        } 
        //output dot
        else if (1 == show->type) {
            DrvLcdSetPixelDot(show->pixel_info.x_startpos,show->pixel_info.y_startpos, show->pixel_info.x_endpos, show->pixel_info.y_endpos,show->pixel_info.pixel_color);
        } else {
            KPrintf("LcdWrite donnot support show type(0 string; 1 dot) %u\n", show->type);
            ret = -ERROR;
        }
    }

    return ret;
}

static uint32 LcdControl(void* drv, struct BusConfigureInfo *configure_info)
{
    x_err_t ret = EOK;

    return ret;
}

static const struct LcdDevDone dev_done  = 
{
    NONE,
    NONE,
    LcdWrite,
    NONE,
};

static int BoardLcdBusInit(struct LcdBus * lcd_bus, struct LcdDriver * lcd_driver,const char *bus_name, const char *drv_name)
{
    x_err_t ret = EOK;

    /*Init the lcd bus */
    ret = LcdBusInit( lcd_bus, bus_name);
    if (EOK != ret) {
        KPrintf("Board_lcd_init LcdBusInit error %d\n", ret);
        return -ERROR;
    }

    /*Init the lcd driver*/
    ret = LcdDriverInit( lcd_driver, drv_name);
    if (EOK != ret) {
        KPrintf("Board_LCD_init LcdDriverInit error %d\n", ret);
        return -ERROR;
    }

    /*Attach the lcd driver to the lcd bus*/
    ret = LcdDriverAttachToBus(drv_name, bus_name);
    if (EOK != ret) {
        KPrintf("Board_LCD_init LcdDriverAttachToBus error %d\n", ret);
        return -ERROR;
    } 

    return ret;
}

/*Attach the lcd device to the lcd bus*/
static int BoardLcdDevBend(struct LcdHardwareDevice *lcd_device, void *param, const char *bus_name, const char *dev_name)
{
    x_err_t ret = EOK;

    ret = LcdDeviceRegister(lcd_device, param, dev_name);
    if (EOK != ret) {
        KPrintf("Board_LCD_init LcdDeviceInit device %s error %d\n", dev_name, ret);
        return -ERROR;
    }  

    ret = LcdDeviceAttachToBus(dev_name, bus_name);
    if (EOK != ret) {
        KPrintf("Board_LCD_init LcdDeviceAttachToBus device %s error %d\n", dev_name, ret);
        return -ERROR;
    }  

    return  ret;
}

int Imxrt1052HwLcdInit(void)
{
    x_err_t ret = EOK;

    static struct LcdBus lcd_bus;
    static struct LcdDriver lcd_drv;
    static struct LcdHardwareDevice lcd_dev;

    memset(&lcd_bus, 0, sizeof(struct LcdBus));
    memset(&lcd_drv, 0, sizeof(struct LcdDriver));
    memset(&lcd_dev, 0, sizeof(struct LcdHardwareDevice));


    lcd_drv.configure = LcdControl;

    ret = BoardLcdBusInit(&lcd_bus, &lcd_drv, LCD_BUS_NAME, LCD_DRV_NAME);
    if (EOK != ret) {
        KPrintf("HwLcdInit BoardLcdBusInit error ret %u\n", ret);
        return -ERROR;
    }

    lcd_dev.dev_done = &dev_done;

    ret = BoardLcdDevBend(&lcd_dev, NONE, LCD_BUS_NAME, LCD_DEVICE_NAME);                             //init  lcd device
    if (EOK != ret) {
        KPrintf("HwLcdInit BoardLcdDevBend error ret %u\n", ret);
        return -ERROR;
    }

    HwLcdInit();

    return ret;
}
