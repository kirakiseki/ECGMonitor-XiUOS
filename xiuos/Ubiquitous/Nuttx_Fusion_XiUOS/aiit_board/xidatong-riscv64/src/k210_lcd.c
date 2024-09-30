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
 * @file k210_lcd.c
 * @brief LCD relative driver
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.7.21
 */


/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include "k210_fpioa.h"
#include "k210_gpiohs.h"
#include "nuttx/arch.h"
#include "nuttx/lcd/lt768.h"
#include "nuttx/lcd/lt768_lib.h"
#include "nuttx/lcd/if_port.h"
#include "nuttx/lcd/lt768_learn.h"
#include <nuttx/board.h>
#include <arch/board/board.h>
#include <nuttx/fs/fs.h>
#include <nuttx/fs/ioctl.h>
#ifdef CONFIG_LCD_LCDDRV_SPIIF
#include "nuttx/lcd/lcddrv_spiif.h"
#endif

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/
#define NCS_H() k210_gpiohs_set_value(FPIOA_LCD_NCS, GPIO_PV_HIGH); up_udelay(20)
#define NCS_L() k210_gpiohs_set_value(FPIOA_LCD_NCS, GPIO_PV_LOW); up_udelay(20)
#define CLK_H() k210_gpiohs_set_value(FPIOA_LCD_SCLK, GPIO_PV_HIGH); up_udelay(20)
#define CLK_L() k210_gpiohs_set_value(FPIOA_LCD_SCLK, GPIO_PV_LOW); up_udelay(20)
#define MOSI_H() k210_gpiohs_set_value(FPIOA_LCD_MOSI, GPIO_PV_HIGH)
#define MOSI_L() k210_gpiohs_set_value(FPIOA_LCD_MOSI, GPIO_PV_LOW)

static int lcd_open(FAR struct file *filep);
static int lcd_close(FAR struct file *filep);
static ssize_t lcd_read(FAR struct file *filep, FAR char *buffer, size_t buflen);
static ssize_t lcd_write(FAR struct file *filep, FAR const char *buffer, size_t buflen);

/****************************************************************************
 * Private Data
 ****************************************************************************/
/* LCD POSIX interface */
static const struct file_operations g_lcdfops =
{
  lcd_open,
  lcd_close,
  lcd_read,
  lcd_write,
  NULL,
  NULL,
  NULL
};
/****************************************************************************
 * Public Functions
 ****************************************************************************/

void lcd_pin_init(void)
{
    k210_fpioa_config(BSP_LCD_NRST, HS_GPIO(FPIOA_LCD_NRST) | K210_IOFLAG_GPIOHS);
    k210_fpioa_config(BSP_LCD_SCLK, HS_GPIO(FPIOA_LCD_SCLK) | K210_IOFLAG_GPIOHS);
    k210_fpioa_config(BSP_LCD_MOSI, HS_GPIO(FPIOA_LCD_MOSI) | K210_IOFLAG_GPIOHS);
    k210_fpioa_config(BSP_LCD_MISO, HS_GPIO(FPIOA_LCD_MISO) | K210_IOFLAG_GPIOHS);
    k210_fpioa_config(BSP_LCD_NCS,  HS_GPIO(FPIOA_LCD_NCS)  | K210_IOFLAG_GPIOHS);

    k210_gpiohs_set_direction(FPIOA_LCD_MISO, GPIO_DM_INPUT);
    k210_gpiohs_set_direction(FPIOA_LCD_NRST, GPIO_DM_OUTPUT);
    k210_gpiohs_set_direction(FPIOA_LCD_SCLK, GPIO_DM_OUTPUT);
    k210_gpiohs_set_direction(FPIOA_LCD_MOSI, GPIO_DM_OUTPUT);
    k210_gpiohs_set_direction(FPIOA_LCD_NCS, GPIO_DM_OUTPUT);

    k210_gpiohs_set_value(FPIOA_LCD_SCLK, GPIO_PV_HIGH);
    k210_gpiohs_set_value(FPIOA_LCD_NCS, GPIO_PV_HIGH);
    k210_gpiohs_set_value(FPIOA_LCD_NRST, GPIO_PV_HIGH);
}

void lcd_backlight_init(bool enable)
{
    k210_fpioa_config(BSP_LCD_BL_PIN, HS_GPIO(FPIOA_LCD_BL) | K210_IOFLAG_GPIOHS);
    k210_gpiohs_set_direction(FPIOA_LCD_BL, GPIO_DM_OUTPUT);
    k210_gpiohs_set_value(FPIOA_LCD_BL, enable);
}

#ifdef CONFIG_LCD_LCDDRV_SPIIF
int spiif_backlight(FAR struct lcddrv_lcd_s *lcd, int level)
{
    lcd_backlight_init(true);
    return 1;
}
#endif

uint8_t lcd_transfer_byte(uint8_t dat)
{
    uint8_t i, rx_data = 0;

    for(i = 0; i < 8; i++)
    {
        CLK_H();

        // MOSI during falling edge
        if((dat << i) & 0x80)
        {
            MOSI_H();
        }
        else
        {
            MOSI_L();
        }

        CLK_L();

        // MISO during rising edge
        rx_data <<= 1;
        if(k210_gpiohs_get_value(FPIOA_LCD_MISO))
            rx_data ++;
    }
    CLK_H();
    return rx_data;
}

void LCD_CmdWrite(uint8_t cmd)
{
    NCS_L();
    lcd_transfer_byte(0x00);
    lcd_transfer_byte(cmd);
    NCS_H();
}

void LCD_DataWrite(uint8_t data)
{
    NCS_L();
    lcd_transfer_byte(0x80);
    lcd_transfer_byte(data);
    NCS_H();
}

void LCD_DataWrite_Pixel(uint8_t data)
{
    NCS_L();
    lcd_transfer_byte(0x80);
    lcd_transfer_byte(data);
    NCS_H();
    NCS_L();
    lcd_transfer_byte(0x80);
    lcd_transfer_byte(data >> 8);
    NCS_H();
}

uint8_t LCD_StatusRead(void)
{
    uint8_t temp = 0;
    NCS_L();
    lcd_transfer_byte(0x40);
    temp = lcd_transfer_byte(0xff);
    NCS_H();
    return temp;
}

uint8_t LCD_DataRead(void)
{
    uint8_t temp = 0;
    NCS_L();
    lcd_transfer_byte(0xc0);
    temp = lcd_transfer_byte(0xff);
    NCS_H();
    return temp;
}

/*****************************************************************************/

void lcd_drv_init(void)
{
    uint8_t PwmControl = 100;

    lcd_pin_init();
    lt768_init();
    Select_SFI_Dual_Mode0();

    // PWM1 enable backlight
    LT768_PWM1_Init(1, 0, 200, 100, PwmControl);

    // enable RGB output
    Display_ON();

    Main_Image_Start_Address(LCD_START_ADDR);
    Main_Image_Width(LCD_XSIZE_TFT);
    Main_Window_Start_XY(0, 0);
    Canvas_Image_Start_address(LCD_START_ADDR);
    Canvas_image_width(LCD_XSIZE_TFT);
    Active_Window_XY(0, 0);
    Active_Window_WH(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
    up_mdelay(10);
    Canvas_Image_Start_address(LCD_START_ADDR);

    //fill blue background
    LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, WHITE);
}

/****************************************************************************
 * Name: k210_backlight
 *
 * Description:
 *   If CONFIG_K210_LCD_BACKLIGHT is defined, then the board-specific
 *   logic must provide this interface to turn the backlight on and off.
 *
 ****************************************************************************/

#ifdef CONFIG_K210_LCD_BACKLIGHT
void k210_backlight(bool blon)
{
    lcd_backlight_init(blon);
}
#endif


/****************************************************************************
 * Name: lcd_open
 ****************************************************************************/
static int lcd_open(FAR struct file *filep)
{
    return OK;
}

/****************************************************************************
 * Name: lcd_close
 ****************************************************************************/
static int lcd_close(FAR struct file *filep)
{
    return OK;
}

/****************************************************************************
 * Name: lcd_read
 ****************************************************************************/
static ssize_t lcd_read(FAR struct file *filep, FAR char *buffer, size_t buflen)
{
    return OK;
}

/****************************************************************************
 * Name: ch438_write
 ****************************************************************************/
 static ssize_t lcd_write(FAR struct file *filep, FAR const char *buffer, size_t buflen)
{
    ssize_t ret = buflen;
    if (buffer  == NULL) 
    {
         return  -ERROR;
    }
    LcdWriteParam * show = (LcdWriteParam *)buffer;

    /* output string */
    switch (show->type)
    {
        
        /* output string */
        case SHOW_STRING:
            LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, WHITE);
            LT768_Select_Internal_Font_Init(show->string_info.height, 1, 1, 1, 1);
            LT768_Print_Internal_Font_String(show->string_info.x_pos, show->string_info.y_pos, show->string_info.font_color,show->string_info.back_color,show->string_info.addr);
            break; 

        /* output dot */
        case SHOW_WDOT:
            LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, WHITE);
            LT768_DrawSquare_Fill(show->pixel_info.x_startpos,show->pixel_info.y_startpos, show->pixel_info.x_endpos, show->pixel_info.y_endpos, *(uint32_t *)(show->pixel_info.pixel_color));
            break;

        /* output rgb */
        case SHOW_RGB:
            LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, WHITE);
            Display_RGB();
            break;

        /* output pip */
        case SHOW_PIP:
            LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, WHITE);
            Display_PIP();
            break;

        /* output Internal Font */
        case SHOW_INTERNAL_FONT:
            LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, WHITE);
            Display_Internal_Font();
            break;

        /* output Outside Font */
        case SHOW_OUTSIDE_FONT:
            LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, WHITE);
            Display_Outside_Font();
            break;

        /* output Triangle */
        case SHOW_TRIANGLE:
            LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, WHITE);
            Display_Triangle();
            break;

        /* output picture */
        case SHOW_PICTURE:
            LT768_DrawSquare_Fill(0, 0, LCD_XSIZE_TFT, LCD_YSIZE_TFT, WHITE);
            Display_Picture();
            break;

        default:
            ret = -ERROR;
            break;
    }

    return ret;
}
/****************************************************************************
 * Name: k210_lcd_initialize
 *
 * Description:
 *   Initialize the LCD.  Setup backlight (initially off)
 *
 ****************************************************************************/
int board_lcd_initialize(void)
{
    /* Configure the LCD backlight (and turn the backlight off) */
    lcd_backlight_init(true);
    lcd_drv_init();
    up_mdelay(10);
    Main_Image_Start_Address(LCD_START_ADDR);
    Main_Image_Width(LCD_XSIZE_TFT);
    Main_Window_Start_XY(0, 0);
    Canvas_Image_Start_address(LCD_START_ADDR);
    Canvas_image_width(LCD_XSIZE_TFT);
    Active_Window_XY(0, 0);
    Active_Window_WH(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
    up_mdelay(10);
    Canvas_Image_Start_address(LCD_START_ADDR);
    /* register device */
    register_driver("/dev/lcd_dev", &g_lcdfops, 0666, NULL);
    return OK;
}
