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
 * @file lt768_lib.c
 * @brief lt768 register relative driver, inherit from Levetop Electronics
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.9.19
 */

#include "nuttx/arch.h"
#include "nuttx/lcd/lt768_lib.h"

/*----------------------------------------------------------------------------*/

uint8_t lt768_cclk; // LT768 clock frequnce
uint8_t lt768_mclk; // SDRAM clock frequnce
uint8_t lt768_sclk; // LCD scan clock frequence

/*----------------------------------------------------------------------------*/

void lt768_pll_init(void)
{
    uint32_t temp = 0;
    uint32_t temp1;
    uint16_t lpllOD_sclk, lpllOD_cclk, lpllOD_mclk;
    uint16_t lpllR_sclk, lpllR_cclk, lpllR_mclk;
    uint16_t lpllN_sclk, lpllN_cclk, lpllN_mclk;
    temp = (LCD_HBPD + LCD_HFPD + LCD_HSPW + LCD_XSIZE_TFT) *
           (LCD_VBPD + LCD_VFPD + LCD_VSPW + LCD_YSIZE_TFT) * 60;
    temp1 = (temp % 1000000) / 100000;

    if(temp1 > 5)
    {
        temp = temp / 1000000 + 1;
    }
    else
    {
        temp = temp / 1000000;
    }

    lt768_sclk = temp;
    temp = temp * 3;
    lt768_mclk = temp;
    lt768_cclk = temp;

    if(lt768_cclk > 100)
    {
        lt768_cclk = 100;
    }

    if(lt768_mclk > 100)
    {
        lt768_mclk = 100;
    }

    if(lt768_sclk > 65)
    {
        lt768_sclk = 65;
    }

#if XI_4M
    lpllOD_sclk = 3;
    lpllOD_cclk = 2;
    lpllOD_mclk = 2;
    lpllR_sclk  = 2;
    lpllR_cclk  = 2;
    lpllR_mclk  = 2;
    lpllN_mclk  = lt768_mclk;
    lpllN_cclk  = lt768_cclk;
    lpllN_sclk  = 2*lt768_sclk;
#endif
#if XI_8M
    lpllOD_sclk = 3;
    lpllOD_cclk = 2;
    lpllOD_mclk = 2;
    lpllR_sclk  = 2;
    lpllR_cclk  = 4;
    lpllR_mclk  = 4;
    lpllN_mclk  = lt768_mclk;
    lpllN_cclk  = lt768_cclk;
    lpllN_sclk  = lt768_sclk;
#endif
#if XI_10M
    lpllOD_sclk = 3;
    lpllOD_cclk = 2;
    lpllOD_mclk = 2;
    lpllR_sclk  = 5;
    lpllR_cclk  = 5;
    lpllR_mclk  = 5;
    lpllN_mclk  = lt768_mclk;
    lpllN_cclk  = lt768_cclk;
    lpllN_sclk  = 2*lt768_sclk;
#endif
#if XI_12M
    lpllOD_sclk = 3;
    lpllOD_cclk = 2;
    lpllOD_mclk = 2;
    lpllR_sclk  = 3;
    lpllR_cclk  = 6;
    lpllR_mclk  = 6;
    lpllN_mclk  = lt768_mclk;
    lpllN_cclk  = lt768_cclk;
    lpllN_sclk  = lt768_sclk;
#endif
    LCD_CmdWrite(0x05);
    LCD_DataWrite((lpllOD_sclk << 6) | (lpllR_sclk << 1) | ((lpllN_sclk >> 8) & 0x1));
    LCD_CmdWrite(0x07);
    LCD_DataWrite((lpllOD_mclk << 6) | (lpllR_mclk << 1) | ((lpllN_mclk >> 8) & 0x1));
    LCD_CmdWrite(0x09);
    LCD_DataWrite((lpllOD_cclk << 6) | (lpllR_cclk << 1) | ((lpllN_cclk >> 8) & 0x1));
    LCD_CmdWrite(0x06);
    LCD_DataWrite(lpllN_sclk);
    LCD_CmdWrite(0x08);
    LCD_DataWrite(lpllN_mclk);
    LCD_CmdWrite(0x0a);
    LCD_DataWrite(lpllN_cclk);
    LCD_CmdWrite(0x00);
    up_udelay(1);
    LCD_DataWrite(0x80);
    up_mdelay(1);
}

void lt768_sdram_init(uint8_t mclk)
{
    uint16_t sdram_itv;
    LCD_RegisterWrite(0xe0, 0x29);
    LCD_RegisterWrite(0xe1, 0x03); //CAS:2=0x02?CAS:3=0x03
    sdram_itv = (64000000 / 8192) / (1000 / mclk) ;
    sdram_itv -= 2;
    LCD_RegisterWrite(0xe2, sdram_itv);
    LCD_RegisterWrite(0xe3, sdram_itv >> 8);
    LCD_RegisterWrite(0xe4, 0x01);
// Check_SDRAM_Ready();
    up_mdelay(1);
}

void lt768_set_panel(void)
{
//**[01h]**//
    TFT_16bit();
//TFT_18bit();
//TFT_24bit();
#if STM32_FSMC_8
    Host_Bus_8bit(); //host bus 8bit
#else
    Host_Bus_16bit(); //host bus 16bit
#endif
//**[02h]**//
    RGB_16b_16bpp();
//RGB_16b_24bpp_mode1();
    MemWrite_Left_Right_Top_Down();
//MemWrite_Down_Top_Left_Right();
//**[03h]**//
    Graphic_Mode();
    Memory_Select_SDRAM();
// PCLK_Falling(); //REG[12h]:falling
    PCLK_Rising();
    VSCAN_T_to_B(); //REG[12h]:from top to bottom
//VSCAN_B_to_T(); //from bottom to top
    PDATA_Set_RGB(); //REG[12h]:Select RGB output
//PDATA_Set_RBG();
//PDATA_Set_GRB();
//PDATA_Set_GBR();
//PDATA_Set_BRG();
//PDATA_Set_BGR();
    HSYNC_Low_Active(); //REG[13h]:
//HSYNC_High_Active();
    VSYNC_Low_Active(); //REG[13h]:
//VSYNC_High_Active();
    DE_High_Active(); //REG[13h]:
//DE_Low_Active();
    LCD_HorizontalWidth_VerticalHeight(LCD_XSIZE_TFT, LCD_YSIZE_TFT);
    LCD_Horizontal_Non_Display(LCD_HBPD);
    LCD_HSYNC_Start_Position(LCD_HFPD);
    LCD_HSYNC_Pulse_Width(LCD_HSPW);
    LCD_Vertical_Non_Display(LCD_VBPD);
    LCD_VSYNC_Start_Position(LCD_VFPD);
    LCD_VSYNC_Pulse_Width(LCD_VSPW);
    Memory_XY_Mode(); //Block mode (X-Y coordination addressing);
    Memory_16bpp_Mode();
}

void lt768_init(void)
{
//Initial_Display_test and set SW2 pin2 = 1
    while(LCD_StatusRead() & 0x02);

    lt768_pll_init();
    lt768_sdram_init(lt768_mclk);
    lt768_set_panel();
}

/*----------------------------------------------------------------------------*/

void MPU8_8bpp_Memory_Write
(
    uint16_t x
    , uint16_t y
    , uint16_t w
    , uint16_t h
    , const uint8_t *data
)
{
    uint16_t i, j;
    Graphic_Mode();
    Active_Window_XY(x, y);
    Active_Window_WH(w, h);
    Goto_Pixel_XY(x, y);
    LCD_CmdWrite(0x04);

    for(i = 0; i < h; i ++)
    {
        for(j = 0; j < w; j ++)
        {
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite(*data);
            data ++;
        }
    }

    Check_Mem_WR_FIFO_Empty();
}


void MPU8_16bpp_Memory_Write
(
    uint16_t x
    , uint16_t y
    , uint16_t w
    , uint16_t h
    , const uint8_t *data
)
{
    uint16_t i, j;
    Graphic_Mode();
    Active_Window_XY(x, y);
    Active_Window_WH(w, h);
    Goto_Pixel_XY(x, y);
    LCD_CmdWrite(0x04);

    for(i = 0; i < h; i ++)
    {
        for(j = 0; j < w; j ++)
        {
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite(*data);
            data ++;
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite(*data);
            data ++;
        }
    }

    Check_Mem_WR_FIFO_Empty();
}


void MPU8_24bpp_Memory_Write
(
    uint16_t x
    , uint16_t y
    , uint16_t w
    , uint16_t h
    , const uint8_t *data
)
{
    uint16_t i, j;
    Graphic_Mode();
    Active_Window_XY(x, y);
    Active_Window_WH(w, h);
    Goto_Pixel_XY(x, y);
    LCD_CmdWrite(0x04);

    for(i = 0; i < h; i ++)
    {
        for(j = 0; j < w; j ++)
        {
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite(*data);
            data ++;
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite(*data);
            data ++;
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite(*data);
            data ++;
        }
    }

    Check_Mem_WR_FIFO_Empty();
}



void MPU16_16bpp_Memory_Write
(
    uint16_t x
    , uint16_t y
    , uint16_t w
    , uint16_t h
    , const uint16_t *data
)
{
    uint16_t i, j;
    Graphic_Mode();
    Active_Window_XY(x, y);
    Active_Window_WH(w, h);
    Goto_Pixel_XY(x, y);
    LCD_CmdWrite(0x04);

    for(i = 0; i < h; i ++)
    {
        for(j = 0; j < w; j ++)
        {
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite_Pixel(*data);
            data ++;
        }
    }

    Check_Mem_WR_FIFO_Empty();
}

void MPU16_24bpp_Mode1_Memory_Write
(
    uint16_t x
    , uint16_t y
    , uint16_t w
    , uint16_t h
    , const uint16_t *data
)
{
    uint16_t i, j;
    Graphic_Mode();
    Active_Window_XY(x, y);
    Active_Window_WH(w, h);
    Goto_Pixel_XY(x, y);
    LCD_CmdWrite(0x04);

    for(i = 0; i < h; i ++)
    {
        for(j = 0; j < w/2; j ++)
        {
            LCD_DataWrite_Pixel(*data);
            Check_Mem_WR_FIFO_not_Full();
            data ++;
            LCD_DataWrite_Pixel(*data);
            Check_Mem_WR_FIFO_not_Full();
            data ++;
            LCD_DataWrite_Pixel(*data);
            Check_Mem_WR_FIFO_not_Full();
            data ++;
        }
    }

    Check_Mem_WR_FIFO_Empty();
}


void MPU16_24bpp_Mode2_Memory_Write
(
    uint16_t x
    , uint16_t y
    , uint16_t w
    , uint16_t h
    , const uint16_t *data
)
{
    uint16_t i, j;
    Graphic_Mode();
    Active_Window_XY(x, y);
    Active_Window_WH(w, h);
    Goto_Pixel_XY(x, y);
    LCD_CmdWrite(0x04);

    for(i = 0; i < h; i ++)
    {
        for(j = 0; j < w; j ++)
        {
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite_Pixel(*data);
            data ++;
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite_Pixel(*data);
            data ++;
        }
    }

    Check_Mem_WR_FIFO_Empty();
}
/*----------------------------------------------------------------------------*/

//------------------------------------- line -----------------------------------------
void LT768_DrawLine
(
    uint16_t X1 // X1 coordination
    , uint16_t Y1 // Y1 coordination
    , uint16_t X2 // X2 coordination
    , uint16_t Y2 // Y2 coordination
    , uint32_t  LineColor // line color
)
{
    Foreground_color_65k(LineColor);
    Line_Start_XY(X1, Y1);
    Line_End_XY(X2, Y2);
    Start_Line();
    Check_2D_Busy();
}

void LT768_DrawLine_Width
(
    uint16_t X1 // X1 coordination
    , uint16_t Y1 // Y1 coordination
    , uint16_t X2 // X2 coordination
    , uint16_t Y2 // Y2 coordination
    , uint32_t  LineColor // line color
    , uint16_t Width // line width
)
{
    uint16_t  i = 0;
    signed  short x = 0, y = 0;
    double temp = 0;
    x = X2 - X1;
    y = Y2 - Y1;

    if(x == 0)
    {
        temp = 2;
    }
    else
    {
        temp = -((double)y/(double)x);
    }

    if(temp >= -1 && temp <= 1)
    {
        while(Width--)
        {
            LT768_DrawLine(X1, Y1 + i, X2, Y2 + i, LineColor);
            i ++;
        }
    }
    else
    {
        while(Width--)
        {
            LT768_DrawLine(X1 + i, Y1, X2 + i, Y2, LineColor);
            i ++;
        }
    }
}

//------------------------------------- circle -----------------------------------------
void LT768_DrawCircle
(
    uint16_t XCenter // center x
    , uint16_t YCenter // center y
    , uint16_t R // radius
    , uint32_t CircleColor // line color
)
{
    Foreground_color_65k(CircleColor);
    Circle_Center_XY(XCenter, YCenter);
    Circle_Radius_R(R);
    Start_Circle_or_Ellipse();
    Check_2D_Busy();
}

void LT768_DrawCircle_Fill
(
    uint16_t XCenter // center x
    , uint16_t YCenter // center y
    , uint16_t R // radius
    , uint32_t ForegroundColor // foreground color
)
{
    Foreground_color_65k(ForegroundColor);
    Circle_Center_XY(XCenter, YCenter);
    Circle_Radius_R(R);
    Start_Circle_or_Ellipse_Fill();
    Check_2D_Busy();
}

void LT768_DrawCircle_Width
(
    uint16_t XCenter // center x
    , uint16_t YCenter // center y
    , uint16_t R // radius
    , uint32_t CircleColor // line color
    , uint32_t ForegroundColor // background color
    , uint16_t Width // line width
)
{
    LT768_DrawCircle_Fill(XCenter, YCenter, R + Width, CircleColor);
    LT768_DrawCircle_Fill(XCenter, YCenter, R, ForegroundColor);
}

//------------------------------------- ellipse -----------------------------------------
void LT768_DrawEllipse
(
    uint16_t XCenter // ellipse center x
    , uint16_t YCenter // ellipse center y
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t EllipseColor // line color
)
{
    Foreground_color_65k(EllipseColor);
    Ellipse_Center_XY(XCenter, YCenter);
    Ellipse_Radius_RxRy(X_R, Y_R);
    Start_Circle_or_Ellipse();
    Check_2D_Busy();
}

void LT768_DrawEllipse_Fill
(
    uint16_t XCenter // ellipse center x
    , uint16_t YCenter // ellipse center y
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t ForegroundColor // background color
)
{
    Foreground_color_65k(ForegroundColor);
    Ellipse_Center_XY(XCenter, YCenter);
    Ellipse_Radius_RxRy(X_R, Y_R);
    Start_Circle_or_Ellipse_Fill();
    Check_2D_Busy();
}

void LT768_DrawEllipse_Width
(
    uint16_t XCenter // ellipse center x
    , uint16_t YCenter // ellipse center y
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t EllipseColor // line color
    , uint32_t ForegroundColor // background color
    , uint16_t Width // line width
)
{
    LT768_DrawEllipse_Fill(XCenter, YCenter, X_R + Width, Y_R + Width, EllipseColor);
    LT768_DrawEllipse_Fill(XCenter, YCenter, X_R, Y_R, ForegroundColor);
}

//------------------------------------- rectangle -----------------------------------------
void LT768_DrawSquare
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint32_t SquareColor // line color
)
{
    Foreground_color_65k(SquareColor);
    Square_Start_XY(X1, Y1);
    Square_End_XY(X2, Y2);
    Start_Square();
    Check_2D_Busy();
}

void LT768_DrawSquare_Fill
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint32_t ForegroundColor // background color
)
{
    Foreground_color_65k(ForegroundColor);
    Square_Start_XY(X1, Y1);
    Square_End_XY(X2, Y2);
    Start_Square_Fill();
    Check_2D_Busy();
}

void LT768_DrawSquare_Width
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint32_t SquareColor // line color
    , uint32_t ForegroundColor // background color
    , uint16_t Width // line width
)
{
    LT768_DrawSquare_Fill(X1-Width, Y1-Width, X2 + Width, Y2 + Width, SquareColor);
    LT768_DrawSquare_Fill(X1, Y1, X2, Y2, ForegroundColor);
}

//------------------------------------- round corner rectangle -----------------------------------------
void LT768_DrawCircleSquare
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t CircleSquareColor // line color
)
{
    Foreground_color_65k(CircleSquareColor);
    Square_Start_XY(X1, Y1);
    Square_End_XY(X2, Y2);
    Circle_Square_Radius_RxRy(X_R, Y_R);
    Start_Circle_Square();
    Check_2D_Busy();
}

void LT768_DrawCircleSquare_Fill
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t ForegroundColor // line color
)
{
    Foreground_color_65k(ForegroundColor);
    Square_Start_XY(X1, Y1);
    Square_End_XY(X2, Y2);
    Circle_Square_Radius_RxRy(X_R, Y_R);
    Start_Circle_Square_Fill();
    Check_2D_Busy();
}

void LT768_DrawCircleSquare_Width
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t CircleSquareColor // line color
    , uint32_t ForegroundColor // line color
    , uint16_t Width
)
{
    LT768_DrawCircleSquare_Fill(X1-Width, Y1-Width, X2 + Width, Y2 + Width, X_R, Y_R, CircleSquareColor);
    LT768_DrawCircleSquare_Fill(X1, Y1, X2, Y2, X_R, Y_R, ForegroundColor);
}

//------------------------------------- triangle -----------------------------------------
void LT768_DrawTriangle
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint16_t X3 // X3 position
    , uint16_t Y3 // Y3 position
    , uint32_t TriangleColor // line color
)
{
    Foreground_color_65k(TriangleColor);
    Triangle_Point1_XY(X1, Y1);
    Triangle_Point2_XY(X2, Y2);
    Triangle_Point3_XY(X3, Y3);
    Start_Triangle();
    Check_2D_Busy();
}

void LT768_DrawTriangle_Fill
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint16_t X3 // X3 position
    , uint16_t Y3 // Y3 position
    , uint32_t ForegroundColor // line color
)
{
    Foreground_color_65k(ForegroundColor);
    Triangle_Point1_XY(X1, Y1);
    Triangle_Point2_XY(X2, Y2);
    Triangle_Point3_XY(X3, Y3);
    Start_Triangle_Fill();
    Check_2D_Busy();
}

void LT768_DrawTriangle_Frame
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint16_t X3 // X3 position
    , uint16_t Y3 // Y3 position
    , uint32_t TriangleColor // line color
    , uint32_t ForegroundColor // background color
)
{
    LT768_DrawTriangle_Fill(X1, Y1, X2, Y2, X3, Y3, ForegroundColor);
    LT768_DrawTriangle(X1, Y1, X2, Y2, X3, Y3, TriangleColor);
}

//------------------------------------- curve -----------------------------------------
void LT768_DrawLeftUpCurve
(
    uint16_t XCenter // center X position
    , uint16_t YCenter // center Y position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t CurveColor // line color
)
{
    Foreground_color_65k(CurveColor);
    Ellipse_Center_XY(XCenter, YCenter);
    Ellipse_Radius_RxRy(X_R, Y_R);
    Start_Left_Up_Curve();
    Check_2D_Busy();
}


void LT768_DrawLeftDownCurve
(
    uint16_t XCenter // center X position
    , uint16_t YCenter // center Y position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t CurveColor // line color
)
{
    Foreground_color_65k(CurveColor);
    Ellipse_Center_XY(XCenter, YCenter);
    Ellipse_Radius_RxRy(X_R, Y_R);
    Start_Left_Down_Curve();
    Check_2D_Busy();
}


void LT768_DrawRightUpCurve
(
    uint16_t XCenter // center X position
    , uint16_t YCenter // center Y position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t CurveColor // line color
)
{
    Foreground_color_65k(CurveColor);
    Ellipse_Center_XY(XCenter, YCenter);
    Ellipse_Radius_RxRy(X_R, Y_R);
    Start_Right_Up_Curve();
    Check_2D_Busy();
}


void LT768_DrawRightDownCurve
(
    uint16_t XCenter // center X position
    , uint16_t YCenter // center Y position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t CurveColor // line color
)
{
    Foreground_color_65k(CurveColor);
    Ellipse_Center_XY(XCenter, YCenter);
    Ellipse_Radius_RxRy(X_R, Y_R);
    Start_Right_Down_Curve();
    Check_2D_Busy();
}


void LT768_SelectDrawCurve
(
    uint16_t XCenter // center X position
    , uint16_t YCenter // center Y position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t CurveColor // line color
    , uint16_t  Dir // direction
)
{
    switch(Dir)
    {
        case 0:
            LT768_DrawLeftDownCurve(XCenter, YCenter, X_R, Y_R, CurveColor);
            break;

        case 1:
            LT768_DrawLeftUpCurve(XCenter, YCenter, X_R, Y_R, CurveColor);
            break;

        case 2:
            LT768_DrawRightUpCurve(XCenter, YCenter, X_R, Y_R, CurveColor);
            break;

        case 3:
            LT768_DrawRightDownCurve(XCenter, YCenter, X_R, Y_R, CurveColor);
            break;

        default:
            break;
    }
}


//------------------------------------- 1/4 filled ellipse -----------------------------------------
void LT768_DrawLeftUpCurve_Fill
(
    uint16_t XCenter // center X position
    , uint16_t YCenter // center Y position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t ForegroundColor // background color
)
{
    Foreground_color_65k(ForegroundColor);
    Ellipse_Center_XY(XCenter, YCenter);
    Ellipse_Radius_RxRy(X_R, Y_R);
    Start_Left_Up_Curve_Fill();
    Check_2D_Busy();
}


void LT768_DrawLeftDownCurve_Fill
(
    uint16_t XCenter // center X position
    , uint16_t YCenter // center Y position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t ForegroundColor // background color
)
{
    Foreground_color_65k(ForegroundColor);
    Ellipse_Center_XY(XCenter, YCenter);
    Ellipse_Radius_RxRy(X_R, Y_R);
    Start_Left_Down_Curve_Fill();
    Check_2D_Busy();
}


void LT768_DrawRightUpCurve_Fill
(
    uint16_t XCenter // center X position
    , uint16_t YCenter // center Y position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t ForegroundColor // background color
)
{
    Foreground_color_65k(ForegroundColor);
    Ellipse_Center_XY(XCenter, YCenter);
    Ellipse_Radius_RxRy(X_R, Y_R);
    Start_Right_Up_Curve_Fill();
    Check_2D_Busy();
}


void LT768_DrawRightDownCurve_Fill
(
    uint16_t XCenter // center X position
    , uint16_t YCenter // center Y position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t ForegroundColor // background color
)
{
    Foreground_color_65k(ForegroundColor);
    Ellipse_Center_XY(XCenter, YCenter);
    Ellipse_Radius_RxRy(X_R, Y_R);
    Start_Right_Down_Curve_Fill();
    Check_2D_Busy();
}


void LT768_SelectDrawCurve_Fill
(
    uint16_t XCenter // center X position
    , uint16_t YCenter // center Y position
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint32_t CurveColor // line color
    , uint16_t  Dir // direction
)
{
    switch(Dir)
    {
        case 0:
            LT768_DrawLeftDownCurve_Fill(XCenter, YCenter, X_R, Y_R, CurveColor);
            break;

        case 1:
            LT768_DrawLeftUpCurve_Fill(XCenter, YCenter, X_R, Y_R, CurveColor);
            break;

        case 2:
            LT768_DrawRightUpCurve_Fill(XCenter, YCenter, X_R, Y_R, CurveColor);
            break;

        case 3:
            LT768_DrawRightDownCurve_Fill(XCenter, YCenter, X_R, Y_R, CurveColor);
            break;

        default:
            break;
    }
}



//------------------------------------- quadrilateral -----------------------------------------

void LT768_DrawQuadrilateral
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint16_t X3 // X3 position
    , uint16_t Y3 // Y3 position
    , uint16_t X4 // X4 position
    , uint16_t Y4 // Y4 position
    , uint32_t ForegroundColor // line color
)
{
    Foreground_color_65k(ForegroundColor);
    Triangle_Point1_XY(X1, Y1);
    Triangle_Point2_XY(X2, Y2);
    Triangle_Point3_XY(X3, Y3);
    Ellipse_Radius_RxRy(X4, Y4);
    LCD_CmdWrite(0x67);
    LCD_DataWrite(0x8d);
    Check_Busy_Draw();
    Check_2D_Busy();
}


void LT768_DrawQuadrilateral_Fill
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint16_t X3 // X3 position
    , uint16_t Y3 // Y3 position
    , uint16_t X4 // X4 position
    , uint16_t Y4 // Y4 position
    , uint32_t ForegroundColor // line color
)
{
    Foreground_color_65k(ForegroundColor);
    Triangle_Point1_XY(X1, Y1);
    Triangle_Point2_XY(X2, Y2);
    Triangle_Point3_XY(X3, Y3);
    Ellipse_Radius_RxRy(X4, Y4);
    LCD_CmdWrite(0x67);
    LCD_DataWrite(0xa7);
    Check_Busy_Draw();
    Check_2D_Busy();
}


//------------------------------------- pentagon -----------------------------------------

void LT768_DrawPentagon
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint16_t X3 // X3 position
    , uint16_t Y3 // Y3 position
    , uint16_t X4 // X4 position
    , uint16_t Y4 // Y4 position
    , uint16_t X5 // X5 position
    , uint16_t Y5 // Y5 position
    , uint32_t ForegroundColor // line color
)
{
    Foreground_color_65k(ForegroundColor);
    Triangle_Point1_XY(X1, Y1);
    Triangle_Point2_XY(X2, Y2);
    Triangle_Point3_XY(X3, Y3);
    Ellipse_Radius_RxRy(X4, Y4);
    Ellipse_Center_XY(X5, Y5);
    LCD_CmdWrite(0x67);
    LCD_DataWrite(0x8F);
    Check_Busy_Draw();
    Check_2D_Busy();
}


void LT768_DrawPentagon_Fill
(
    uint16_t X1 // X1 position
    , uint16_t Y1 // Y1 position
    , uint16_t X2 // X2 position
    , uint16_t Y2 // Y2 position
    , uint16_t X3 // X3 position
    , uint16_t Y3 // Y3 position
    , uint16_t X4 // X4 position
    , uint16_t Y4 // Y4 position
    , uint16_t X5 // X5 position
    , uint16_t Y5 // Y5 position
    , uint32_t ForegroundColor // line color
)
{
    Foreground_color_65k(ForegroundColor);
    Triangle_Point1_XY(X1, Y1);
    Triangle_Point2_XY(X2, Y2);
    Triangle_Point3_XY(X3, Y3);
    Ellipse_Radius_RxRy(X4, Y4);
    Ellipse_Center_XY(X5, Y5);
    LCD_CmdWrite(0x67);
    LCD_DataWrite(0xa9);
    Check_Busy_Draw();
    Check_2D_Busy();
}


//------------------------------------- cylinder -----------------------------------------
uint8_t LT768_DrawCylinder
(
    uint16_t XCenter // ellipse center x
    , uint16_t YCenter // ellipse center y
    , uint16_t X_R // radius width
    , uint16_t Y_R // radius length
    , uint16_t H
    , uint32_t CylinderColor // line color
    , uint32_t ForegroundColor // background color
)
{
    if(YCenter < H)
    {
        return 1;
    }

//bottom ellipse
    LT768_DrawEllipse_Fill(XCenter, YCenter, X_R, Y_R, ForegroundColor);
    LT768_DrawEllipse(XCenter, YCenter, X_R, Y_R, CylinderColor);
//middle rectangle
    LT768_DrawSquare_Fill(XCenter-X_R, YCenter-H, XCenter + X_R, YCenter, ForegroundColor);
//top ellipse
    LT768_DrawEllipse_Fill(XCenter, YCenter-H, X_R, Y_R, ForegroundColor);
    LT768_DrawEllipse(XCenter, YCenter-H, X_R, Y_R, CylinderColor);
    LT768_DrawLine(XCenter-X_R, YCenter, XCenter-X_R, YCenter-H, CylinderColor);
    LT768_DrawLine(XCenter + X_R, YCenter, XCenter + X_R, YCenter-H, CylinderColor);
    return 0;
}


//------------------------------------- quadrangular -----------------------------------------
void LT768_DrawQuadrangular
(
    uint16_t X1
    , uint16_t Y1
    , uint16_t X2
    , uint16_t Y2
    , uint16_t X3
    , uint16_t Y3
    , uint16_t X4
    , uint16_t Y4
    , uint16_t X5
    , uint16_t Y5
    , uint16_t X6
    , uint16_t Y6
    , uint32_t QuadrangularColor // line color
    , uint32_t ForegroundColor // background color
)
{
    LT768_DrawSquare_Fill(X1, Y1, X5, Y5, ForegroundColor);
    LT768_DrawSquare(X1, Y1, X5, Y5, QuadrangularColor);
    LT768_DrawQuadrilateral_Fill(X1, Y1, X2, Y2, X3, Y3, X4, Y4, ForegroundColor);
    LT768_DrawQuadrilateral(X1, Y1, X2, Y2, X3, Y3, X4, Y4, QuadrangularColor);
    LT768_DrawQuadrilateral_Fill(X3, Y3, X4, Y4, X5, Y5, X6, Y6, ForegroundColor);
    LT768_DrawQuadrilateral(X3, Y3, X4, Y4, X5, Y5, X6, Y6, QuadrangularColor);
}


//-------------------------------------- table ------------------------------------
void LT768_MakeTable
(
    uint16_t X1, // start position X1
    uint16_t Y1, // start position X2
    uint16_t W,
    uint16_t H,
    uint16_t Line, // line number
    uint16_t Row, // row number
    uint32_t  TableColor, // table color C1
    uint32_t  ItemColor, // item background color C2
    uint32_t  ForegroundColor, // foreground color C3
    uint16_t width1, // internal width
    uint16_t width2, // out-side width
    uint8_t  mode // 0: vertical     1: cross
)
{
    uint16_t i = 0;
    uint16_t x2, y2;
    x2 = X1 + W * Row;
    y2 = Y1 + H * Line;
    LT768_DrawSquare_Width(X1, Y1, x2, y2, TableColor, ForegroundColor, width2);

    if(mode == 0)
    {
        LT768_DrawSquare_Fill(X1, Y1, X1 + W, y2, ItemColor);
    }
    else if(mode == 1)
    {
        LT768_DrawSquare_Fill(X1, Y1, x2, Y1 + H, ItemColor);
    }

    for(i = 0 ; i < Line ; i ++)
    {
        LT768_DrawLine_Width(X1, Y1 + i*H, x2, Y1 + i*H, TableColor, width1);
    }

    for(i = 0 ; i < Row ; i ++)
    {
        LT768_DrawLine_Width(X1 + i*W, Y1, X1 + i*W, y2, TableColor, width1);
    }
}

/*----------------------------------------------------------------------------*/

void LT768_Color_Bar_ON(void)
{
    Color_Bar_ON();
}

void LT768_Color_Bar_OFF(void)
{
    Color_Bar_OFF();
}

/*----------------------------------------------------------------------------*/

/*--Flash-->DMA-->SDRAM--*/
/*address:24bit or 32bit*/

void LT768_DMA_24bit_Linear
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint32_t flash_addr // flash start address
    , uint32_t memory_addr // memory start address
    , uint32_t data_num // data number
)
{
    Enable_SFlash_SPI(); // enable SPI

    if(SCS == 0)
    {
        Select_SFI_0(); // select extern SPI0
    }

    if(SCS == 1)
    {
        Select_SFI_1(); // select extern SPI1
    }

    Memory_Linear_Mode();
    Select_SFI_DMA_Mode(); // set SPI DMA mode
    SPI_Clock_Period(Clk); // SPI speed
    SFI_DMA_Destination_Start_Address(memory_addr); // destination start address
    SFI_DMA_Transfer_Number(data_num); // DMA transport number
    SFI_DMA_Source_Start_Address(flash_addr); // flash address
    Check_Busy_SFI_DMA();
    Start_SFI_DMA();
    Check_Busy_SFI_DMA();
    Memory_XY_Mode();
}

void LT768_DMA_32bit_Linear
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint32_t flash_addr // flash start address
    , uint32_t memory_addr // memory start address
    , uint32_t data_num // data number
)
{
    Enable_SFlash_SPI(); // enable SPI

    if(SCS == 0)
    {
        Select_SFI_0(); // select extern SPI0
    }

    if(SCS == 1)
    {
        Select_SFI_1(); // select extern SPI1
    }

    Memory_Linear_Mode();
    Select_SFI_DMA_Mode(); // set SPI DMA mode
    Select_SFI_32bit_Address();
    SPI_Clock_Period(Clk); // SPI speed
    SFI_DMA_Destination_Start_Address(memory_addr); // destination start address
    SFI_DMA_Transfer_Number(data_num); // DMA transport number
    SFI_DMA_Source_Start_Address(flash_addr); // flash address
    Check_Busy_SFI_DMA();
    Start_SFI_DMA();
    Check_Busy_SFI_DMA();
    Memory_XY_Mode();
}

void LT768_DMA_24bit_Block
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint16_t X1 // transport X1 address
    , uint16_t Y1 // transport Y1 address
    , uint16_t X_W // DMA transport width
    , uint16_t Y_H // DMA transport length
    , uint16_t P_W // picture width
    , uint32_t Addr // flash address
)
{
    Enable_SFlash_SPI(); // enable SPI

    if(SCS == 0)
    {
        Select_SFI_0(); // select extern SPI0
    }

    if(SCS == 1)
    {
        Select_SFI_1(); // select extern SPI1
    }

    Select_SFI_DMA_Mode(); // set SPI DMA mode
    SPI_Clock_Period(Clk); // set SPI clock frequence divsion factor
    Goto_Pixel_XY(X1, Y1); // set memory address
    SFI_DMA_Destination_Upper_Left_Corner(X1, Y1); // DMA destination address
    SFI_DMA_Transfer_Width_Height(X_W, Y_H); // set block width and length
    SFI_DMA_Source_Width(P_W); // set source data width
    SFI_DMA_Source_Start_Address(Addr); // set source data in flash address
    Start_SFI_DMA(); // start DMA transport
    Check_Busy_SFI_DMA(); // check DMA transport finish
}


void LT768_DMA_32bit_Block
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint16_t X1 // X1 address
    , uint16_t Y1 // Y1 address
    , uint16_t X_W // DMA transport width
    , uint16_t Y_H // DMA transport length
    , uint16_t P_W // picture width
    , uint32_t Addr // flash address
)
{
    Enable_SFlash_SPI();

    if(SCS == 0)
    {
        Select_SFI_0();
    }

    if(SCS == 1)
    {
        Select_SFI_1();
    }

    Select_SFI_DMA_Mode();
    SPI_Clock_Period(Clk);
    Select_SFI_32bit_Address();
    Goto_Pixel_XY(X1, Y1);
    SFI_DMA_Destination_Upper_Left_Corner(X1, Y1);
    SFI_DMA_Transfer_Width_Height(X_W, Y_H);
    SFI_DMA_Source_Width(P_W);
    SFI_DMA_Source_Start_Address(Addr);
    Start_SFI_DMA();
    Check_Busy_SFI_DMA();
    Select_SFI_24bit_Address();
}

/*----------------------------------------------------------------------------*/
/* select integrated font initialization */
void LT768_Select_Internal_Font_Init
(
    uint8_t Size // set word size   16:16*16     24:24*24    32:32*32
    , uint8_t XxN // word width enlargement factor: 1~4
    , uint8_t YxN // word length enlargement factor: 1~4
    , uint8_t ChromaKey // 0:font background color transparent    1:set font background color
    , uint8_t Alignment // 0:no font no align      1:font align
)
{
    if(Size == 16)
    {
        Font_Select_8x16_16x16();
    }

    if(Size == 24)
    {
        Font_Select_12x24_24x24();
    }

    if(Size == 32)
    {
        Font_Select_16x32_32x32();
    }

//(*)
    if(XxN == 1)
    {
        Font_Width_X1();
    }

    if(XxN == 2)
    {
        Font_Width_X2();
    }

    if(XxN == 3)
    {
        Font_Width_X3();
    }

    if(XxN == 4)
    {
        Font_Width_X4();
    }

//(*)
    if(YxN == 1)
    {
        Font_Height_X1();
    }

    if(YxN == 2)
    {
        Font_Height_X2();
    }

    if(YxN == 3)
    {
        Font_Height_X3();
    }

    if(YxN == 4)
    {
        Font_Height_X4();
    }

//(*)
    if(ChromaKey == 0)
    {
        Font_Background_select_Color();
    }

    if(ChromaKey == 1)
    {
        Font_Background_select_Transparency();
    }

//(*)
    if(Alignment == 0)
    {
        Disable_Font_Alignment();
    }

    if(Alignment == 1)
    {
        Enable_Font_Alignment();
    }
}


/* print integrated font*/
void LT768_Print_Internal_Font_String
(
    uint16_t x // font start x
    , uint16_t y // font start y
    , uint32_t FontColor // font color
    , uint32_t BackGroundColor // font background color(when font background is transparent, it is invalid)
    , uint8_t *c // data start address
)
{
    Text_Mode();
    CGROM_Select_Internal_CGROM();
    Foreground_color_65k(FontColor);
    Background_color_65k(BackGroundColor);
    Goto_Text_XY(x, y);
    Show_String(c);
}


/* select integrated font initialization */
void LT768_Select_Outside_Font_Init
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint32_t FlashAddr // source address(Flash)
    , uint32_t MemoryAddr // destination address(SDRAM)
    , uint32_t Num // word lib size
    , uint8_t Size // set word size   16:16*16     24:24*24    32:32*32
    , uint8_t XxN // word width enlargement factor: 1~4
    , uint8_t YxN // word length enlargement factor: 1~4
    , uint8_t ChromaKey // 0:font background color transparent    1:set font background color
    , uint8_t Alignment // 0:no font no align      1:font align
)
{
    if(Size == 16)
    {
        Font_Select_8x16_16x16();
    }

    if(Size == 24)
    {
        Font_Select_12x24_24x24();
    }

    if(Size == 32)
    {
        Font_Select_16x32_32x32();
    }

//(*)
    if(XxN == 1)
    {
        Font_Width_X1();
    }

    if(XxN == 2)
    {
        Font_Width_X2();
    }

    if(XxN == 3)
    {
        Font_Width_X3();
    }

    if(XxN == 4)
    {
        Font_Width_X4();
    }

//(*)
    if(YxN == 1)
    {
        Font_Height_X1();
    }

    if(YxN == 2)
    {
        Font_Height_X2();
    }

    if(YxN == 3)
    {
        Font_Height_X3();
    }

    if(YxN == 4)
    {
        Font_Height_X4();
    }

//(*)
    if(ChromaKey == 0)
    {
        Font_Background_select_Color();
    }

    if(ChromaKey == 1)
    {
        Font_Background_select_Transparency();
    }

//(*)
    if(Alignment == 0)
    {
        Disable_Font_Alignment();
    }

    if(Alignment == 1)
    {
        Enable_Font_Alignment();
    }

    LT768_DMA_24bit_Linear(SCS, Clk, FlashAddr, MemoryAddr, Num);
    CGRAM_Start_address(MemoryAddr);
}

/* print outsize and integrated font  */
void LT768_Print_Outside_Font_String
(
    uint16_t x // font start x
    , uint16_t y // font start y
    , uint32_t FontColor // font color
    , uint32_t BackGroundColor // font background color((when font background is transparent, it is invalid))
    , uint8_t *c // data start address
)
{
    uint16_t temp_H = 0;
    uint16_t temp_L = 0;
    uint16_t temp = 0;
    uint32_t i = 0;
    Text_Mode();
    Font_Select_UserDefine_Mode();
    Foreground_color_65k(FontColor);
    Background_color_65k(BackGroundColor);
    Goto_Text_XY(x, y);

    while(c[i] != '\0')
    {
        if(c[i] < 0xa1)
        {
            CGROM_Select_Internal_CGROM(); // fond source use inside CGROM
            LCD_CmdWrite(0x04);
            LCD_DataWrite(c[i]);
            Check_Mem_WR_FIFO_not_Full();
            i += 1;
        }
        else
        {
            Font_Select_UserDefine_Mode(); // user define font
            LCD_CmdWrite(0x04);
            temp_H = ((c[i] - 0xa1) & 0x00ff) * 94;
            temp_L = c[i + 1] - 0xa1;
            temp = temp_H + temp_L + 0x8000;
            LCD_DataWrite((temp>>8)&0xff);
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite(temp&0xff);
            Check_Mem_WR_FIFO_not_Full();
            i += 2;
        }
    }

    Check_2D_Busy();
    Graphic_Mode(); //back to graphic mode;
}

/* print external GBK and integrated font  */
void LT768_Print_Outside_Font_GBK_String
(
    uint16_t x // font start x
    , uint16_t y // font start y
    , uint32_t FontColor // font color
    , uint32_t BackGroundColor // font background color((when font background is transparent, it is invalid))
    , uint8_t *c // data start address
)
{
    uint16_t temp_H = 0;
    uint16_t temp_L = 0;
    uint16_t temp = 0;
    uint32_t i = 0;
    Text_Mode();
    Font_Select_UserDefine_Mode();
    Foreground_color_65k(FontColor);
    Background_color_65k(BackGroundColor);
    Goto_Text_XY(x, y);

    while(c[i] != '\0')
    {
        if(c[i] < 0x81)
        {
            CGROM_Select_Internal_CGROM(); // fond source use inside CGROM
            LCD_CmdWrite(0x04);
            LCD_DataWrite(c[i]);
            Check_Mem_WR_FIFO_not_Full();
            i += 1;
        }
        else
        {
            Font_Select_UserDefine_Mode(); // user define font
            LCD_CmdWrite(0x04);
            temp_H = ((c[i] - 0x81) & 0x00ff) * 191;
            temp_L = c[i + 1] - 0x40;
            temp = temp_H + temp_L + 0x8000;
            LCD_DataWrite((temp>>8)&0xff);
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite(temp&0xff);
            Check_Mem_WR_FIFO_not_Full();
            i += 2;
        }
    }

    Check_2D_Busy();
    Graphic_Mode(); //back to graphic mode;
}

/*print 48*48,72*72 font */
void LT768_BTE_Memory_Copy_ColorExpansion_8
(
    uint32_t S0_Addr // SO memory start address
    , uint16_t YS0 // SO top-left Y coordinate
    , uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X(show window start X)
    , uint16_t YDes // destination top-left Y(show window start y)
    , uint16_t X_W // show window width
    , uint16_t Y_H // show window length
    , uint32_t Foreground_color
    , uint32_t Background_color
)
{
    Foreground_color_256(Foreground_color);
    Background_color_256(Background_color);
    BTE_ROP_Code(7);
    BTE_S0_Color_8bpp();
    BTE_S0_Memory_Start_Address(S0_Addr);
    BTE_S0_Image_Width(Des_W);
    BTE_S0_Window_Start_XY(YS0, 0);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_Operation_Code(0x0e); //BTE Operation: Memory copy (move) with chroma keying (w/o ROP)
    BTE_Window_Size(X_W, Y_H);
    BTE_Enable();
    Check_BTE_Busy();
}

void LT768_BTE_Memory_Copy_ColorExpansion_Chroma_key_8
(
    uint32_t S0_Addr // SO memory start address
    , uint16_t YS0 // SO top-left Y coordinate
    , uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X(show window start X)
    , uint16_t YDes // destination top-left Y(show window start y)
    , uint16_t X_W // show window width
    , uint16_t Y_H // show window length
    , uint32_t Foreground_color
)
{
    Foreground_color_256(Foreground_color);
    BTE_ROP_Code(7);
    BTE_S0_Color_8bpp();
    BTE_S0_Memory_Start_Address(S0_Addr);
    BTE_S0_Image_Width(Des_W);
    BTE_S0_Window_Start_XY(YS0, 0);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_Operation_Code(0x0f); //BTE Operation: Memory copy (move) with chroma keying (w/o ROP)
    BTE_Window_Size(X_W, Y_H);
    BTE_Enable();
    Check_BTE_Busy();
}

void LT768_Print_Outside_Font_GB2312_48_72
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint32_t FlashAddr // font source address(Flash)
    , uint32_t MemoryAddr // destination address(SDRAM)
    , uint32_t ShowAddr // show layer address
    , uint16_t width // show layer width
    , uint8_t Size // set word size   48:48*48     72:72*72
    , uint8_t ChromaKey // 0:font background color transparent    1:set font background color
    , uint16_t x // font start x
    , uint16_t y // font start y
    , uint32_t FontColor // font color
    , uint32_t BackGroundColor // font background color((when font background is transparent, it is invalid))
    , uint16_t w // font thinkness: 0: not overstriking 1: 1 overstriking 2: 2 overstriking
    , uint16_t s // linewidth
    , uint8_t *c // data start address
)
{
    uint16_t temp_H = 0;
    uint16_t temp_L = 0;
    uint16_t temp = 0;
    uint32_t i = 0;
    uint16_t j = 0;
    uint16_t k = 0;
    uint16_t h = 0;
    uint16_t n = 0;
    uint16_t m = 0;
    uint16_t g = 0;
    uint16_t f = 0;
    h = x;
    k = y;
    Memory_8bpp_Mode();//save picture with 8bit color depth
    Canvas_Image_Start_address(MemoryAddr);
    Canvas_image_width(width);

    while(c[i] != '\0')
    {
        temp_H = (c[i] - 0xa1) * 94;
        temp_L = c[i + 1] - 0xa1;
        temp = temp_H + temp_L;
        LT768_DMA_24bit_Block(SCS, Clk, Size * j / 8, 0, Size / 8, Size, Size / 8,
                              FlashAddr + temp * ((Size * Size) / 8));
        i += 2;
        j ++;
    }

    Memory_16bpp_Mode();
    Canvas_Image_Start_address(ShowAddr);
    Canvas_image_width(width);
    j = 0;
    i = 0;

    if(w > 2)
    {
        w = 2;
    }

    for(g=0; g<w + 1; g ++)
    {
        while(c[i] != '\0')
        {
            if((f == m) && ((x + Size*j + Size)>(width*(n + 1))))
            {
                m ++;
                n ++;
                y=y + Size-1 + s;
                x = x + ((width*n)-(x + Size*j)) + g;
                f=n;
            }

            if(ChromaKey == 1)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_8(
                    MemoryAddr, Size * j / 8,
                    ShowAddr, width, x + Size * j, y,
                    Size, Size, FontColor, BackGroundColor
                );
            }

            if(ChromaKey == 0)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_Chroma_key_8(
                    MemoryAddr, Size * j / 8,
                    ShowAddr, width, x + Size * j, y,
                    Size, Size, FontColor
                );
            }

            i += 2;
            j ++;
        }

        ChromaKey=0;
        i = 0;
        j = 0;
        m = 0;
        n = 0;
        f = 0;
        x = h + g + 1;
        y = k + g + 1;
    }
}


void LT768_Print_Outside_Font_BIG5_48_72
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint32_t FlashAddr // font source address(Flash)
    , uint32_t MemoryAddr // destination address(SDRAM)
    , uint32_t ShowAddr // show layer address
    , uint16_t width // show layer width
    , uint8_t Size // set word size   48:48*48     72:72*72
    , uint8_t ChromaKey // 0:font background color transparent    1:set font background color
    , uint16_t x // font start x
    , uint16_t y // font start y
    , uint32_t FontColor // font color
    , uint32_t BackGroundColor // font background color((when font background is transparent, it is invalid))
    , uint16_t w // font thinkness: 0: not overstriking 1: 1 overstriking 2: 2 overstriking
    , uint16_t s // linewidth
    , uint8_t *c // data start address
)
{
    uint16_t temp_H = 0;
    uint16_t temp_L = 0;
    uint16_t temp = 0;
    uint32_t i = 0;
    uint16_t j = 0;
    uint16_t k = 0;
    uint16_t h = 0;
    uint16_t n = 0;
    uint16_t m = 0;
    uint16_t g = 0;
    uint16_t f = 0;
    h = x;
    k = y;
    Memory_8bpp_Mode();//save picture with 8bit color depth
    Canvas_Image_Start_address(MemoryAddr);
    Canvas_image_width(width);

    while(c[i] != '\0')
    {
        temp_H = (c[i] - 0xa1) * 160;

        if(c[i + 1]<0x7f)
        {
            temp_L = c[i + 1] - 0x40;
        }
        else
        {
            temp_L = c[i + 1] - 0xa0 + 0x40;
        }

        temp = temp_H + temp_L;
        LT768_DMA_24bit_Block(SCS, Clk, Size * j / 8, 0, Size / 8, Size, Size / 8,
                              FlashAddr + temp * ((Size * Size) / 8));
        i += 2;
        j ++;
    }

    Memory_16bpp_Mode(); // print 16bits color depth
    Canvas_Image_Start_address(ShowAddr);
    Canvas_image_width(width);
    j = 0;
    i = 0;

    for(g=0; g<w; g ++)
    {
        while(c[i] != '\0')
        {
            if((f == m) && ((x + Size*j + Size)>(1024*(n + 1))))
            {
                m ++;
                n ++;
                y=y + Size-1 + s;
                x = x + ((1024*n)-(x + Size*j)) + g;
                f=n;
            }

            if(ChromaKey == 1)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_8(
                    MemoryAddr, Size * j / 8,
                    ShowAddr, LCD_XSIZE_TFT, x + Size * j, y,
                    Size, Size, FontColor, BackGroundColor
                );
            }

            if(ChromaKey == 0)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_Chroma_key_8(
                    MemoryAddr, Size * j / 8,
                    ShowAddr, LCD_XSIZE_TFT, x + Size * j, y,
                    Size, Size, FontColor
                );
            }

            i += 2;
            j ++;
        }

        ChromaKey=0;
        i = 0;
        j = 0;
        m = 0;
        n = 0;
        f = 0;
        x = h + g + 1;
        y = k + g + 1;
    }
}

void LT768_Print_Outside_Font_GBK_48_72
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint32_t FlashAddr // font source address(Flash)
    , uint32_t MemoryAddr // destination address(SDRAM)
    , uint32_t ShowAddr // show layer address
    , uint16_t width // show layer width
    , uint8_t Size // set word size   48:48*48     72:72*72
    , uint8_t ChromaKey // 0:font background color transparent    1:set font background color
    , uint16_t x // font start x
    , uint16_t y // font start y
    , uint32_t FontColor // font color
    , uint32_t BackGroundColor // font background color((when font background is transparent, it is invalid))
    , uint16_t w // font thinkness: 0: not overstriking 1: 1 overstriking 2: 2 overstriking
    , uint16_t s // linewidth
    , uint8_t *c // data start address
)
{
    uint16_t temp_H = 0;
    uint16_t temp_L = 0;
    uint16_t temp = 0;
    uint32_t i = 0;
    uint16_t j = 0;
    uint16_t k = 0;
    uint16_t h = 0;
    uint16_t n = 0;
    uint16_t m = 0;
    uint16_t g = 0;
    uint16_t f = 0;
    h = x;
    k = y;
    Memory_8bpp_Mode();//save picture with 8bit color depth
    Canvas_Image_Start_address(MemoryAddr);
    Canvas_image_width(width);

    while(c[i] != '\0')
    {
        temp_H = (c[i] - 0x81) * 191;
        temp_L = c[i + 1] - 0x40;
        temp = temp_H + temp_L;
        LT768_DMA_24bit_Block(SCS, Clk, Size * j / 8, 0, Size / 8, Size, Size / 8,
                              FlashAddr + temp * ((Size * Size) / 8));
        i += 2;
        j ++;
    }

    Memory_16bpp_Mode();
    Canvas_Image_Start_address(ShowAddr);
    Canvas_image_width(width);
    j = 0;
    i = 0;

    if(w > 2)
    {
        w = 2;
    }

    for(g=0; g<w + 1; g ++)
    {
        while(c[i] != '\0')
        {
            if((f == m) && ((x + Size*j + Size)>(width*(n + 1))))
            {
                m ++;
                n ++;
                y=y + Size-1 + s;
                x = x + ((width*n)-(x + Size*j)) + g;
                f=n;
            }

            if(ChromaKey == 1)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_8(
                    MemoryAddr, Size * j / 8,
                    ShowAddr, width, x + Size * j, y,
                    Size, Size, FontColor, BackGroundColor
                );
            }

            if(ChromaKey == 0)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_Chroma_key_8(
                    MemoryAddr, Size * j / 8,
                    ShowAddr, width, x + Size * j, y,
                    Size, Size, FontColor
                );
            }

            i += 2;
            j ++;
        }

        ChromaKey = 0;
        i = 0;
        j = 0;
        m = 0;
        n = 0;
        f = 0;
        x = h + g + 1;
        y = k + g + 1;
    }
}

/*user-defined font*/
//only input chinese font
#define Font_size 200 //define font size
#define Font_BYTE Font_size * 2
uint8_t Font_1[Font_BYTE];

void clear_Font_1(void)
{
    uint16_t i;

    for(i = 0; i < Font_BYTE; i ++)
    {
        Font_1[i] = 0;
    }
}

int Get_User_Font_P(char chH, char chL)
{
    int i;

    for(i = 0; i <(sizeof(Font_1)/2); i ++)
    {
        if((chH == Font_1[i * 2]) && (chL == Font_1[i * 2 + 1]))
        {
            return i;
        }
    }

    return -1;
}

void LT768_Print_Outside_UserDefineFont_GB2312
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint32_t FlashAddr // font source address(Flash)
    , uint32_t MemoryAddr // destination address(SDRAM)
    , uint32_t ShowAddr // show layer address
    , uint16_t width // show layer width
    , uint8_t Size // set word size   48:48*48     72:72*72
    , uint8_t ChromaKey // 0:font background color transparent    1:set font background color
    , uint16_t x // font start x
    , uint16_t y // font start y
    , uint32_t FontColor // font color
    , uint32_t BackGroundColor // font background color((when font background is transparent, it is invalid))
    , uint16_t w // font thinkness: 0: not overstriking 1: 1 overstriking 2: 2 overstriking
    , uint16_t s // linewidth
    , uint8_t *c // data start address
)
{
    short temp = 0;
    uint32_t i = 0;
    uint16_t j = 0;
    uint16_t k = 0;
    uint16_t h = 0;
    uint16_t n = 0;
    uint16_t m = 0;
    uint16_t g = 0;
    uint16_t f = 0;
    uint8_t temp1[2] = {0};
    uint32_t fontNum = 0;
    clear_Font_1();
    LT768_SPI_Init(SCS, 1);
    LT_W25QXX_Read(temp1, FlashAddr + 8, 2);
    fontNum = (temp1[0] << 8) + temp1[1];
    LT_W25QXX_Read(Font_1, FlashAddr + 0xc, fontNum * 2);
    h = x;
    k = y;
    Memory_8bpp_Mode();//save picture with 8bit color depth
    Canvas_Image_Start_address(MemoryAddr);
    Canvas_image_width(width);

    while(c[i] != '\0')
    {
        temp = Get_User_Font_P(c[i], c[i + 1]);

        if(temp>-1)
        {
            LT768_DMA_24bit_Block(SCS, Clk, Size * j / 8, 0, Size / 8, Size, Size / 8,
                                  FlashAddr + temp*((Size*Size)/8) + 0x0c + fontNum*2);
        }

        i += 2;
        j ++;
    }

    Memory_16bpp_Mode();
    Canvas_Image_Start_address(ShowAddr);
    Canvas_image_width(width);
    j = 0;
    i = 0;

    if(w > 2)
    {
        w = 2;
    }

    for(g=0; g<w + 1; g ++)
    {
        while(c[i] != '\0')
        {
            if((f == m) && ((x + Size*j + Size)>(width*(n + 1))))
            {
                m ++;
                n ++;
                y=y + Size-1 + s;
                x = x + ((width*n)-(x + Size*j)) + g;
                f=n;
            }

            if(ChromaKey == 1)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_8(
                    MemoryAddr, Size * j / 8,
                    ShowAddr, width, x + Size * j, y,
                    Size, Size, FontColor, BackGroundColor
                );
            }

            if(ChromaKey == 0)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_Chroma_key_8(
                    MemoryAddr, Size * j / 8,
                    ShowAddr, width, x + Size * j, y,
                    Size, Size, FontColor
                );
            }

            i += 2;
            j ++;
        }

        ChromaKey = 0;
        i = 0;
        j = 0;
        m = 0;
        n = 0;
        f = 0;
        x = h + g + 1;
        y = k + g + 1;
    }
}

/*----------------------------------------------------------------------------*/

void LT768_Text_cursor_Init
(
    uint8_t On_Off_Blinking // 0: disable 1: enable blink
    , uint16_t Blinking_Time // set time of blink
    , uint16_t X_W // font cursor width
    , uint16_t Y_W // font cursor length
)
{
    if(On_Off_Blinking == 0)
    {
        Disable_Text_Cursor_Blinking();
    }

    if(On_Off_Blinking == 1)
    {
        Enable_Text_Cursor_Blinking();
    }

    Blinking_Time_Frames(Blinking_Time);
//[3E][3Fh]
    Text_Cursor_H_V(X_W, Y_W);
    Enable_Text_Cursor();
}


void LT768_Enable_Text_Cursor(void)
{
    Enable_Text_Cursor();
}


void LT768_Disable_Text_Cursor(void)
{
    Disable_Text_Cursor();
}


void LT768_Graphic_cursor_Init
(
    uint8_t Cursor_N // select cursor 1 2 3 4
    , uint8_t Color1 // color1
    , uint8_t Color2 // color2
    , uint16_t X_Pos // print coordinationX
    , uint16_t Y_Pos // print coordinationY
    , uint8_t *Cursor_Buf // cursor buffer start address
)
{
    uint32_t i ;
    Memory_Select_Graphic_Cursor_RAM();
    Graphic_Mode();

    switch(Cursor_N)
    {
        case 1:
            Select_Graphic_Cursor_1();
            break;

        case 2:
            Select_Graphic_Cursor_2();
            break;

        case 3:
            Select_Graphic_Cursor_3();
            break;

        case 4:
            Select_Graphic_Cursor_4();
            break;

        default:
            break;
    }

    LCD_CmdWrite(0x04);

    for(i = 0; i < 256; i ++)
    {
        LCD_DataWrite(Cursor_Buf[i]);
    }

    Memory_Select_SDRAM();//go back to SDRAM after write finish
    Set_Graphic_Cursor_Color_1(Color1);
    Set_Graphic_Cursor_Color_2(Color2);
    Graphic_Cursor_XY(X_Pos, Y_Pos);
    Enable_Graphic_Cursor();
}


void LT768_Set_Graphic_cursor_Pos
(
    uint8_t Cursor_N // select cursor 1 2 3 4
    , uint16_t X_Pos // print coordinationX
    , uint16_t Y_Pos // print coordinationY
)
{
    Graphic_Cursor_XY(X_Pos, Y_Pos);

    switch(Cursor_N)
    {
        case 1:
            Select_Graphic_Cursor_1();
            break;

        case 2:
            Select_Graphic_Cursor_2();
            break;

        case 3:
            Select_Graphic_Cursor_3();
            break;

        case 4:
            Select_Graphic_Cursor_4();
            break;

        default:
            break;
    }
}


void LT768_Enable_Graphic_Cursor(void)
{
    Enable_Graphic_Cursor();
}


void LT768_Disable_Graphic_Cursor(void)
{
    Disable_Graphic_Cursor();
}

/*----------------------------------------------------------------------------*/

void LT768_PIP_Init
(
    uint8_t On_Off // 0 : disable PIP    1 :  enable PIP    2 :  keep
    , uint8_t Select_PIP // 1 : use PIP1   2 : use PIP2
    , uint32_t PAddr // PIP start address
    , uint16_t XP // PIP window X coordiation must divisible by 4
    , uint16_t YP // // PIP window Y coordiation must divisible by 4
    , uint32_t ImageWidth // image width
    , uint16_t X_Dis // print windows X coordination
    , uint16_t Y_Dis // print windows Y coordination
    , uint16_t X_W // show window width, must be divisible by 4
    , uint16_t Y_H // show window length, must be divisible by 4
)
{
    if(Select_PIP == 1)
    {
        Select_PIP1_Window_16bpp();
        Select_PIP1_Parameter();
    }

    if(Select_PIP == 2)
    {
        Select_PIP2_Window_16bpp();
        Select_PIP2_Parameter();
    }

    PIP_Display_Start_XY(X_Dis, Y_Dis);
    PIP_Image_Start_Address(PAddr);
    PIP_Image_Width(ImageWidth);
    PIP_Window_Image_Start_XY(XP, YP);
    PIP_Window_Width_Height(X_W, Y_H);

    if(On_Off == 0)
    {
        if(Select_PIP == 1)
        {
            Disable_PIP1();
        }

        if(Select_PIP == 2)
        {
            Disable_PIP2();
        }
    }

    if(On_Off == 1)
    {
        if(Select_PIP == 1)
        {
            Enable_PIP1();
        }

        if(Select_PIP == 2)
        {
            Enable_PIP2();
        }
    }
}


void LT768_Set_DisWindowPos
(
    uint8_t On_Off // 0: disable PIP, 1: enable PIP, 2: keep
    , uint8_t Select_PIP // 1: use PIP1, 2: use PIP2
    , uint16_t X_Dis // print windows X coordination
    , uint16_t Y_Dis // print windows Y coordination
)
{
    if(Select_PIP == 1)
    {
        Select_PIP1_Parameter();
    }

    if(Select_PIP == 2)
    {
        Select_PIP2_Parameter();
    }

    if(On_Off == 0)
    {
        if(Select_PIP == 1)
        {
            Disable_PIP1();
        }

        if(Select_PIP == 2)
        {
            Disable_PIP2();
        }
    }

    if(On_Off == 1)
    {
        if(Select_PIP == 1)
        {
            Enable_PIP1();
        }

        if(Select_PIP == 2)
        {
            Enable_PIP2();
        }
    }

    PIP_Display_Start_XY(X_Dis, Y_Dis);
}

/*----------------------------------------------------------------------------*/

void BTE_Solid_Fill
(
    uint32_t Des_Addr // fill destination address
    , uint16_t Des_W // destination picture width
    , uint16_t XDes
    , uint16_t YDes
    , uint16_t color // fill color
    , uint16_t X_W // fill length
    , uint16_t Y_H // fill width
)
{
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_Window_Size(X_W, Y_H);
    Foreground_color_65k(color);
    BTE_Operation_Code(0x0c);
    BTE_Enable();
    Check_BTE_Busy();
}

/* BTE memory copy with raster operation*/
void LT768_BTE_Memory_Copy
(
    uint32_t S0_Addr // SO memory start address
    , uint16_t S0_W // SO memory width
    , uint16_t XS0 // SO top-left X coordinate
    , uint16_t YS0 // SO top-left Y coordinate
    , uint32_t S1_Addr // S1 memory start address
    , uint16_t S1_W // S1 width
    , uint16_t XS1 // S1 top-left X
    , uint16_t YS1 // S1 top-left Y
    , uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left Y
    , uint32_t ROP_Code // raster operation mode
    /*ROP_Code :
       0000b    0(Blackness)
       0001b    ~S0!E~S1 or ~(S0 + S1)
       0010b    ~S0!ES1
       0011b    ~S0
       0100b    S0!E~S1
       0101b    ~S1
       0110b    S0^S1
       0111b    ~S0 + ~S1 or ~(S0 + S1)
       1000b    S0!ES1
       1001b    ~(S0^S1)
       1010b    S1
       1011b    ~S0 + S1
       1100b    S0
       1101b    S0 + ~S1
       1110b    S0 + S1
       1111b    1(whiteness)*/
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
)
{
    BTE_S0_Color_16bpp();
    BTE_S0_Memory_Start_Address(S0_Addr);
    BTE_S0_Image_Width(S0_W);
    BTE_S0_Window_Start_XY(XS0, YS0);
    BTE_S1_Color_16bpp();
    BTE_S1_Memory_Start_Address(S1_Addr);
    BTE_S1_Image_Width(S1_W);
    BTE_S1_Window_Start_XY(XS1, YS1);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_ROP_Code(ROP_Code);
    BTE_Operation_Code(0x02); //BTE Operation: Memory copy (move) with ROP.
    BTE_Window_Size(X_W, Y_H);
    BTE_Enable();
    Check_BTE_Busy();
}


/* memory copy with Chroma Key (not include ROP) */
void LT768_BTE_Memory_Copy_Chroma_key
(
    uint32_t S0_Addr // SO memory start address
    , uint16_t S0_W // SO memory width
    , uint16_t XS0 // SO top-left X coordinate
    , uint16_t YS0 // SO top-left Y coordinate
    , uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left X
    , uint32_t Background_color // transparent color
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
)
{
    Background_color_65k(Background_color);
    BTE_S0_Color_16bpp();
    BTE_S0_Memory_Start_Address(S0_Addr);
    BTE_S0_Image_Width(S0_W);
    BTE_S0_Window_Start_XY(XS0, YS0);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_Operation_Code(0x05); //BTE Operation: Memory copy (move) with chroma keying (w/o ROP)
    BTE_Window_Size(X_W, Y_H);
    BTE_Enable();
    Check_BTE_Busy();
}


void LT768_BTE_Pattern_Fill
(
    uint8_t P_8x8_or_16x16 // 0 : use 8x8 Icon , 1 : use 16x16 Icon.
    , uint32_t S0_Addr // SO memory start address
    , uint16_t S0_W // SO memory width
    , uint16_t XS0 // SO top-left X coordinate
    , uint16_t YS0 // SO top-left Y coordinate
    , uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left X
    , uint32_t ROP_Code // raster operation mode
    /*ROP_Code :
       0000b    0(Blackness)
       0001b    ~S0!E~S1 or ~(S0 + S1)
       0010b    ~S0!ES1
       0011b    ~S0
       0100b    S0!E~S1
       0101b    ~S1
       0110b    S0^S1
       0111b    ~S0 + ~S1 or ~(S0 + S1)
       1000b    S0!ES1
       1001b    ~(S0^S1)
       1010b    S1
       1011b    ~S0 + S1
       1100b    S0
       1101b    S0 + ~S1
       1110b    S0 + S1
       1111b    1(whiteness)*/
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
)
{
    if(P_8x8_or_16x16 == 0)
    {
        Pattern_Format_8X8();
    }

    if(P_8x8_or_16x16 == 1)
    {
        Pattern_Format_16X16();
    }

    BTE_S0_Color_16bpp();
    BTE_S0_Memory_Start_Address(S0_Addr);
    BTE_S0_Image_Width(S0_W);
    BTE_S0_Window_Start_XY(XS0, YS0);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_ROP_Code(ROP_Code);
    BTE_Operation_Code(0x06); //BTE Operation: Pattern Fill with ROP.
    BTE_Window_Size(X_W, Y_H);
    BTE_Enable();
    Check_BTE_Busy();
}


void LT768_BTE_Pattern_Fill_With_Chroma_key
(
    uint8_t P_8x8_or_16x16 // 0 : use 8x8 Icon , 1 : use 16x16 Icon.
    , uint32_t S0_Addr // SO memory start address
    , uint16_t S0_W // SO memory width
    , uint16_t XS0 // SO top-left X coordinate
    , uint16_t YS0 // SO top-left Y coordinate
    , uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left Y
    , uint32_t ROP_Code // raster operation mode
    /*ROP_Code :
       0000b    0(Blackness)
       0001b    ~S0!E~S1 or ~(S0 + S1)
       0010b    ~S0!ES1
       0011b    ~S0
       0100b    S0!E~S1
       0101b    ~S1
       0110b    S0^S1
       0111b    ~S0 + ~S1 or ~(S0 + S1)
       1000b    S0!ES1
       1001b    ~(S0^S1)
       1010b    S1
       1011b    ~S0 + S1
       1100b    S0
       1101b    S0 + ~S1
       1110b    S0 + S1
       1111b    1(whiteness)*/
    , uint32_t Background_color // transparent color
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
)
{
    Background_color_65k(Background_color);

    if(P_8x8_or_16x16 == 0)
    {
        Pattern_Format_8X8();
    }

    if(P_8x8_or_16x16 == 1)
    {
        Pattern_Format_16X16();
    }

    BTE_S0_Color_16bpp();
    BTE_S0_Memory_Start_Address(S0_Addr);
    BTE_S0_Image_Width(S0_W);
    BTE_S0_Window_Start_XY(XS0, YS0);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_ROP_Code(ROP_Code);
    BTE_Operation_Code(0x07); //BTE Operation: Pattern Fill with Chroma key.
    BTE_Window_Size(X_W, Y_H);
    BTE_Enable();
    Check_BTE_Busy();
}

void LT768_BTE_MCU_Write_MCU_16bit
(
    uint32_t S1_Addr // S1 memory start address
    , uint16_t S1_W // S1 width
    , uint16_t XS1 // S1 top-left X
    , uint16_t YS1 // S1 top-left Y
    , uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left Y
    , uint32_t ROP_Code // raster operation mode
    /*ROP_Code :
       0000b    0(Blackness)
       0001b    ~S0!E~S1 or ~(S0 + S1)
       0010b    ~S0!ES1
       0011b    ~S0
       0100b    S0!E~S1
       0101b    ~S1
       0110b    S0^S1
       0111b    ~S0 + ~S1 or ~(S0 + S1)
       1000b    S0!ES1
       1001b    ~(S0^S1)
       1010b    S1
       1011b    ~S0 + S1
       1100b    S0
       1101b    S0 + ~S1
       1110b    S0 + S1
       1111b    1(whiteness)*/
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
    , const uint16_t *data // S0 start address
)
{
    uint16_t i, j;
    BTE_S1_Color_16bpp();
    BTE_S1_Memory_Start_Address(S1_Addr);
    BTE_S1_Image_Width(S1_W);
    BTE_S1_Window_Start_XY(XS1, YS1);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_Window_Size(X_W, Y_H);
    BTE_ROP_Code(ROP_Code);
    BTE_Operation_Code(0x00); //BTE Operation: MPU Write with ROP.
    BTE_Enable();
    BTE_S0_Color_16bpp();
    LCD_CmdWrite(0x04); //Memory Data Read/Write Port

//MCU_16bit_ColorDepth_16bpp
    for(i = 0; i < Y_H; i ++)
    {
        for(j = 0; j < (X_W); j ++)
        {
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite_Pixel((*data));
            data ++;
        }
    }

    Check_Mem_WR_FIFO_Empty();
    Check_BTE_Busy();
}


void LT768_BTE_MCU_Write_Chroma_key_MCU_16bit
(
    uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left Y
    , uint32_t Background_color // transparent color
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
    , const uint16_t *data // S0 receive address
)
{
    uint32_t i, j;
    Background_color_65k(Background_color);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_Window_Size(X_W, Y_H);
    BTE_Operation_Code(0x04); //BTE Operation: MPU Write with chroma keying (w/o ROP)
    BTE_Enable();
    BTE_S0_Color_16bpp();
    LCD_CmdWrite(0x04); //Memory Data Read/Write Port

//MCU_16bit_ColorDepth_16bpp
    for(i = 0; i < Y_H; i ++)
    {
        for(j = 0; j < (X_W); j ++)
        {
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite_Pixel((*data));
            data ++;
        }
    }

    Check_Mem_WR_FIFO_Empty();
    Check_BTE_Busy();
}


/* MPU write with expansion color */
void LT768_BTE_MCU_Write_ColorExpansion_MCU_16bit
(
    uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left Y
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
    , uint32_t Foreground_color // foreground color
    /*Foreground_color : The source (1bit map picture) map data 1 translate to Foreground color by color expansion*/
    , uint32_t Background_color // background color
    /*Background_color : The source (1bit map picture) map data 0 translate to Background color by color expansion*/
    , const uint16_t *data // buffer start address
)
{
    uint16_t i, j;
    RGB_16b_16bpp();
    Foreground_color_65k(Foreground_color);
    Background_color_65k(Background_color);
    BTE_ROP_Code(15);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_Window_Size(X_W, Y_H);
    BTE_Operation_Code(0x8); //BTE Operation: MPU Write with Color Expansion (w/o ROP)
    BTE_Enable();
    LCD_CmdWrite(0x04); //Memory Data Read/Write Port

    for(i = 0; i < Y_H; i ++)
    {
        for(j = 0; j < X_W/16; j ++)
        {
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite_Pixel(*data);
            data ++;
        }
    }

    Check_Mem_WR_FIFO_Empty();
    Check_BTE_Busy();
}

/* MPU write with expansion color and chroma key */
void LT768_BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_16bit
(
    uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left Y
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
    , uint32_t Foreground_color // foreground color
    /*Foreground_color : The source (1bit map picture) map data 1 translate to Foreground color by color expansion*/
    , const uint16_t *data // buffer start address
)
{
    uint16_t i, j;
    RGB_16b_16bpp();
    Foreground_color_65k(Foreground_color);
    BTE_ROP_Code(15);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_Window_Size(X_W, Y_H);
    BTE_Operation_Code(0x9); //BTE Operation: MPU Write with Color Expansion and chroma keying (w/o ROP)
    BTE_Enable();
    LCD_CmdWrite(0x04); //Memory Data Read/Write Port

    for(i = 0; i < Y_H; i ++)
    {
        for(j = 0; j < X_W/16; j ++)
        {
            Check_Mem_WR_FIFO_not_Full();
            LCD_DataWrite_Pixel(*data);
            data ++;
        }
    }

    Check_Mem_WR_FIFO_Empty();
    Check_BTE_Busy();
}

/* memory copy with transparent */
void BTE_Alpha_Blending
(
    uint32_t S0_Addr // SO memory start address
    , uint16_t S0_W // SO memory width
    , uint16_t XS0 // SO top-left X coordinate
    , uint16_t YS0 // SO top-left Y coordinate
    , uint32_t S1_Addr // S1 memory start address
    , uint16_t S1_W // S1 width
    , uint16_t XS1 // S1 top-left X
    , uint16_t YS1 // S1 top-left Y
    , uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left X
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
    , uint8_t alpha // transparent grade (32)
)
{
    BTE_S0_Color_16bpp();
    BTE_S0_Memory_Start_Address(S0_Addr);
    BTE_S0_Image_Width(S0_W);
    BTE_S0_Window_Start_XY(XS0, YS0);
    BTE_S1_Color_16bpp();
    BTE_S1_Memory_Start_Address(S1_Addr);
    BTE_S1_Image_Width(S1_W);
    BTE_S1_Window_Start_XY(XS1, YS1);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_Window_Size(X_W, Y_H);
    BTE_Operation_Code(0x0A); //BTE Operation: Memory write with opacity (w/o ROP)
    BTE_Alpha_Blending_Effect(alpha);
    BTE_Enable();
    Check_BTE_Busy();
}

void BTE_Pixel_8bpp_Alpha_Blending
(
    uint32_t S0_Addr // SO memory start address
    , uint16_t S0_W // SO memory width
    , uint16_t XS0 // SO top-left X coordinate
    , uint16_t YS0 // SO top-left Y coordinate
    , uint32_t S1_Addr // S1 memory start address
    , uint16_t S1_W // S1 width
    , uint16_t XS1 // S1 top-left X
    , uint16_t YS1 // S1 top-left Y
    , uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left X
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
)
{
    BTE_S0_Color_8bpp();
    BTE_S0_Memory_Start_Address(S0_Addr);
    BTE_S0_Image_Width(S0_W);
    BTE_S0_Window_Start_XY(XS0, YS0);
    BTE_S1_Color_8bit_Alpha();
    BTE_S1_Memory_Start_Address(S1_Addr);
    BTE_S1_Image_Width(S1_W);
    BTE_S1_Window_Start_XY(XS1, YS1);
    BTE_Destination_Color_8bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_Window_Size(X_W, Y_H);
    BTE_Operation_Code(0x0A); //BTE Operation: Memory write with opacity (w/o ROP)
    BTE_Enable();
    Check_BTE_Busy();
}

void BTE_Pixel_16bpp_Alpha_Blending
(
    uint32_t S0_Addr // SO memory start address
    , uint16_t S0_W // SO memory width
    , uint16_t XS0 // SO top-left X coordinate
    , uint16_t YS0 // SO top-left Y coordinate
    , uint32_t S1_Addr // S1 memory start address
    , uint16_t S1_W // S1 width
    , uint16_t XS1 // S1 top-left X
    , uint16_t YS1 // S1 top-left Y
    , uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left X
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
)
{
    BTE_S0_Color_16bpp();
    BTE_S0_Memory_Start_Address(S0_Addr);
    BTE_S0_Image_Width(S0_W);
    BTE_S0_Window_Start_XY(XS0, YS0);
    BTE_S1_Color_16bit_Alpha();
    BTE_S1_Memory_Start_Address(S1_Addr);
    BTE_S1_Image_Width(S1_W);
    BTE_S1_Window_Start_XY(XS1, YS1);
    BTE_Destination_Color_16bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_Window_Size(X_W, Y_H);
    BTE_Operation_Code(0x0A); //BTE Operation: Memory write with opacity (w/o ROP)
    BTE_Enable();
    Check_BTE_Busy();
}

/*----------------------------------------------------------------------------*/

void LT768_PWM0_Init
(
    uint8_t on_off // 0: enable PWM0 1: enable PWM0
    , uint8_t Clock_Divided // PWM clock frequency division range 0~3(1, 1/2, 1/4, 1/8)
    , uint8_t Prescalar // clock frequency range 1~256
    , uint16_t Count_Buffer // PWM output period
    , uint16_t Compare_Buffer // set duty ratio
)
{
    Select_PWM0();
    Set_PWM_Prescaler_1_to_256(Prescalar);

    if(Clock_Divided  == 0)
    {
        Select_PWM0_Clock_Divided_By_1();
    }

    if(Clock_Divided  == 1)
    {
        Select_PWM0_Clock_Divided_By_2();
    }

    if(Clock_Divided  == 2)
    {
        Select_PWM0_Clock_Divided_By_4();
    }

    if(Clock_Divided  == 3)
    {
        Select_PWM0_Clock_Divided_By_8();
    }

    Set_Timer0_Count_Buffer(Count_Buffer);
    Set_Timer0_Compare_Buffer(Compare_Buffer);

    if(on_off == 1)
    {
        Start_PWM0();
    }

    if(on_off == 0)
    {
        Stop_PWM0();
    }
}


void LT768_PWM0_Duty(uint16_t Compare_Buffer)
{
    Set_Timer0_Compare_Buffer(Compare_Buffer);
}

void LT768_PWM1_Init
(
    uint8_t on_off // 0: enable PWM0 1: enable PWM0
    , uint8_t Clock_Divided // PWM clock frequency division range 0~3(1, 1/2, 1/4, 1/8)
    , uint8_t Prescalar // clock frequency range 1~256
    , uint16_t Count_Buffer // PWM output period
    , uint16_t Compare_Buffer // set duty ratio
)
{
    Select_PWM1();
    Set_PWM_Prescaler_1_to_256(Prescalar);

    if(Clock_Divided  == 0)
    {
        Select_PWM1_Clock_Divided_By_1();
    }

    if(Clock_Divided  == 1)
    {
        Select_PWM1_Clock_Divided_By_2();
    }

    if(Clock_Divided  == 2)
    {
        Select_PWM1_Clock_Divided_By_4();
    }

    if(Clock_Divided  == 3)
    {
        Select_PWM1_Clock_Divided_By_8();
    }

    Set_Timer1_Count_Buffer(Count_Buffer);
    Set_Timer1_Compare_Buffer(Compare_Buffer);

    if(on_off == 1)
    {
        Start_PWM1();
    }

    if(on_off == 0)
    {
        Stop_PWM1();
    }
}


void LT768_PWM1_Duty(uint16_t Compare_Buffer)
{
    Set_Timer1_Compare_Buffer(Compare_Buffer);
}

/*----------------------------------------------------------------------------*/

// LT768 enter standby mode
void LT768_Standby(void)
{
    Power_Saving_Standby_Mode();
    Check_Power_is_Saving();
}

// wakeup from standby mode
void LT768_Wkup_Standby(void)
{
    Power_Normal_Mode();
    Check_Power_is_Normal();
}

// LT768 enter suspend mode
void LT768_Suspend(void)
{
    lt768_sdram_init(10);
    Power_Saving_Suspend_Mode();
    Check_Power_is_Saving();
}

//wakeup from suspend mode
void LT768_Wkup_Suspend(void)
{
    Power_Normal_Mode();
    Check_Power_is_Normal();
    lt768_sdram_init(lt768_mclk);
}

// LT768 enter sleep mode
void LT768_SleepMode(void)
{
    Power_Saving_Sleep_Mode();
    Check_Power_is_Saving();
}

// wakeup from sleep mode
void LT768_Wkup_Sleep(void)
{
    Power_Normal_Mode();
    Check_Power_is_Normal();
}

/*----------------------------------------------------------------------------*/

void LT768_SPI_Init(uint8_t cs, uint8_t div)
{
    if(cs == 0)
    {
        Select_SFI_0();
        Select_nSS_drive_on_xnsfcs0();
    }
    else
    {
        Select_SFI_1();
        Select_nSS_drive_on_xnsfcs1();
    }

    SPI_Clock_Period(div);
    Enable_SFlash_SPI();
}

void W25QXX_Enter_4Byte_AddressMode(void)//val:select external SPI: SCS:0  SCS:1
{
    nSS_Active();
    SPI_Master_FIFO_Data_Put(0xB7);
    nSS_Inactive();
}

void LT_W25QXX_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    uint16_t i;
    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x03);
    SPI_Master_FIFO_Data_Put((uint8_t)((ReadAddr)>>16));
    SPI_Master_FIFO_Data_Put((uint8_t)((ReadAddr)>>8));
    SPI_Master_FIFO_Data_Put((uint8_t)ReadAddr);

    for(i = 0; i <NumByteToRead; i ++)
    {
        pBuffer[i] = SPI_Master_FIFO_Data_Put(0XFF);
    }

    nSS_Inactive();
}

/*----------------------------------------------------------------------------*/

uint8_t W25N01GV_ReadSR(uint8_t reg)
{
    uint8_t byte = 0;
    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x0f);
    SPI_Master_FIFO_Data_Put(reg);
    byte = SPI_Master_FIFO_Data_Put(0Xff);
    nSS_Inactive();
    return byte;
}

void W25N01GV_Write_SR(uint8_t reg, uint8_t val)
{
    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x1f);
    SPI_Master_FIFO_Data_Put(reg);
    SPI_Master_FIFO_Data_Put(val);
    nSS_Inactive();
}

void W25N01GV_Wait_Busy(void)
{
    while((W25N01GV_ReadSR(0xC0)&0x01) == 0x01);
}

void W25N01GV_ContinuousRead_Mode(void)
{
    uint8_t statut = 0;
    statut = W25N01GV_ReadSR(0xb0);
    statut &= cClrb3;
    statut &= cClrb4;
    statut |= cSetb4;
    W25N01GV_Write_SR(0xb0, statut);
}

void W25N01GV_Write_Page(uint16_t page)
{
    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x13);
    SPI_Master_FIFO_Data_Put(0x00);
    SPI_Master_FIFO_Data_Put((uint8_t)(page >> 8));
    SPI_Master_FIFO_Data_Put((uint8_t)page);
    nSS_Inactive();
    W25N01GV_Wait_Busy();
}

void W25N01GV_ReadPageAddr_Data(uint8_t *pBuffer, uint32_t PageNum, uint32_t PageAddr, uint16_t NumByteToRead)
{
    uint16_t i;
//------read data from flash to buffer-----
    nSS_Active(); //enable
    SPI_Master_FIFO_Data_Put(0x13);
    SPI_Master_FIFO_Data_Put(0x00);
    SPI_Master_FIFO_Data_Put((uint8_t)((PageNum) >> 8)); //send 16bit address
    SPI_Master_FIFO_Data_Put((uint8_t)(PageNum));
    nSS_Inactive();
    W25N01GV_Wait_Busy();
    W25N01GV_Write_SR(0xb0, W25N01GV_ReadSR(0xb0) | 0x08); //set read position of buffer
//------read data from buffer to MCU-----
    nSS_Active();
    SPI_Master_FIFO_Data_Put(0x03);
    SPI_Master_FIFO_Data_Put((PageAddr)>>8);
    SPI_Master_FIFO_Data_Put(PageAddr);
    SPI_Master_FIFO_Data_Put(0x00);

    for(i = 0; i <NumByteToRead; i ++)
    {
        pBuffer[i]=SPI_Master_FIFO_Data_Put(0XFF); //cycle read number
    }

    nSS_Active();
    W25N01GV_Wait_Busy();
}

void LT_W25N01GV_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)//read data under 2k, from page 0
{
    uint16_t page, endpage, remainder;
    remainder = ReadAddr % 2048;//picture start position at first page
    page = (ReadAddr / 2048);//picture start position page index (page address)
    endpage = ((ReadAddr + NumByteToRead) / 2048);//picture end position page index (page address)
    LT768_SPI_Init(1, 1);

    if(page == endpage)
    {
        W25N01GV_ReadPageAddr_Data(pBuffer, page, remainder, NumByteToRead);
    }
    else
    {
        W25N01GV_ReadPageAddr_Data(pBuffer, page, remainder, 2048 - remainder);
        W25N01GV_ReadPageAddr_Data(&pBuffer[2048 - remainder], endpage, 0, NumByteToRead - (2048 - remainder));
    }
}

void LT768_Nand_Pic
(
    uint8_t SCS
    , uint16_t X1 // print start X coordination
    , uint16_t Y1 // print start Y coordination
    , uint16_t X_W // picture width
    , uint16_t Y_H // picture high
    , uint32_t Addr // start address on flash
    , uint32_t lay0 // print area which has handled picture
    , uint32_t lay1 // buffer1 which save handled picture
)
{
    uint16_t page, endpage, remainder, X_W4 = 0;
    remainder = Addr % 2048;//picture start position at first page
    page = (Addr / 2048);//picture start position page index (page address)

    if(X_W % 4 != 0)
    {
        X_W4 = (X_W / 4) * 4 + 4;
    }
    else
    {
        X_W4 = X_W;
    }

    endpage = ((Addr + X_W4 * Y_H * 2)/2048);//picture end position page index (page address)
    LT768_SPI_Init(SCS, 1);
    W25N01GV_ContinuousRead_Mode();
    W25N01GV_Write_Page(page);
    LT768_DMA_24bit_Linear(1, 1, 0, lay1 - remainder, ((endpage-page + 1) * 2048));// transport all data of the page to layer 2
    Canvas_Image_Start_address(lay0);
    Canvas_image_width(LCD_XSIZE_TFT);
    LT768_BTE_Memory_Copy(lay1, X_W4, 0, 0,
                          lay1, X_W4, 0, 0,
                          lay0, LCD_XSIZE_TFT, X1, Y1,
                          0x0c, X_W, Y_H);
}

void NandFlash_Select_Outside_Font_Init
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint32_t FlashAddr // source address(Flash)
    , uint32_t MemoryAddr // destination address(SDRAM)
    , uint32_t Num // word lib size
    , uint8_t Size // set word size   16:16*16     24:24*24    32:32*32
    , uint8_t XxN // word width enlargement factor: 1~4
    , uint8_t YxN // word length enlargement factor: 1~4
    , uint8_t ChromaKey // 0:font background color transparent    1:set font background color
    , uint8_t Alignment // 0:no font no align      1:font align
)
{
    uint16_t page, endpage, remainder;
    remainder = FlashAddr % 2048;//font at one page start position
    page = (FlashAddr / 2048);//font start address page index (page address)
    endpage = ((FlashAddr + Num) / 2048);//font end address page index (page address)
    LT768_SPI_Init(1, 1);
    W25N01GV_ContinuousRead_Mode();
    W25N01GV_Write_Page(page);

    if(Size == 16)
    {
        Font_Select_8x16_16x16();
    }

    if(Size == 24)
    {
        Font_Select_12x24_24x24();
    }

    if(Size == 32)
    {
        Font_Select_16x32_32x32();
    }

//(*)
    if(XxN == 1)
    {
        Font_Width_X1();
    }

    if(XxN == 2)
    {
        Font_Width_X2();
    }

    if(XxN == 3)
    {
        Font_Width_X3();
    }

    if(XxN == 4)
    {
        Font_Width_X4();
    }

//(*)
    if(YxN == 1)
    {
        Font_Height_X1();
    }

    if(YxN == 2)
    {
        Font_Height_X2();
    }

    if(YxN == 3)
    {
        Font_Height_X3();
    }

    if(YxN == 4)
    {
        Font_Height_X4();
    }

    if(ChromaKey == 0)
    {
        Font_Background_select_Color();
    }

    if(ChromaKey == 1)
    {
        Font_Background_select_Transparency();
    }

    if(Alignment == 0)
    {
        Disable_Font_Alignment();
    }

    if(Alignment == 1)
    {
        Enable_Font_Alignment();
    }

    LT768_DMA_24bit_Linear(SCS, Clk, FlashAddr, MemoryAddr, (endpage-page + 1)*2048);
    CGRAM_Start_address(MemoryAddr + remainder);
}

void LT768_BTE_Memory_Copy_8
(
    uint32_t S0_Addr // SO memory start address
    , uint16_t S0_W // SO memory width
    , uint16_t XS0 // SO top-left X coordinate
    , uint16_t YS0 // SO top-left Y coordinate
    , uint32_t S1_Addr // S1 memory start address
    , uint16_t S1_W // S1 width
    , uint16_t XS1 // S1 top-left X
    , uint16_t YS1 // S1 top-left Y
    , uint32_t Des_Addr // destination memory start address
    , uint16_t Des_W // destination memory width
    , uint16_t XDes // destination top-left X
    , uint16_t YDes // destination top-left Y
    , uint32_t ROP_Code // raster operation mode
    , uint16_t X_W // active windows width
    , uint16_t Y_H // active windows length
)
{
    BTE_S0_Color_8bpp();
    BTE_S0_Memory_Start_Address(S0_Addr);
    BTE_S0_Image_Width(S0_W);
    BTE_S0_Window_Start_XY(XS0, YS0);
    BTE_S1_Color_8bpp();
    BTE_S1_Memory_Start_Address(S1_Addr);
    BTE_S1_Image_Width(S1_W);
    BTE_S1_Window_Start_XY(XS1, YS1);
    BTE_Destination_Color_8bpp();
    BTE_Destination_Memory_Start_Address(Des_Addr);
    BTE_Destination_Image_Width(Des_W);
    BTE_Destination_Window_Start_XY(XDes, YDes);
    BTE_ROP_Code(ROP_Code);
    BTE_Operation_Code(0x02); //BTE Operation: Memory copy (move) with ROP.
    BTE_Window_Size(X_W, Y_H);
    BTE_Enable();
    Check_BTE_Busy();
}


void LT768_Nand_8bpp_font
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint16_t X1 // transport X1 address
    , uint16_t Y1 // transport Y1 address
    , uint16_t X_W // DMA transport width
    , uint16_t Y_H // DMA transport length
    , uint32_t Addr // flash address
    , uint32_t lay1 // cache layer 1 address
    , uint32_t lay2 // cache layer 2 address
)
{
    uint16_t i, page, endpage, remainder, x1, y1;
    remainder = Addr % 2048;//picture start position at first page
    page = (Addr / 2048);//picture start position page index (page address)
    endpage = ((Addr + X_W * Y_H) / 2048);//picture end position page index (page address)
    LT768_SPI_Init(1, 1);
    W25N01GV_ContinuousRead_Mode();
    W25N01GV_Write_Page(page);
    LT768_DMA_24bit_Linear(1, 1, 0, lay1, ((endpage-page + 1) * 2048));// insert picture page all data
    x1 = (remainder) % LCD_XSIZE_TFT;
    y1 = (remainder) / LCD_XSIZE_TFT;

//re-organization
    for(i = 0 ; i < Y_H ; i ++)
    {
        LT768_BTE_Memory_Copy_8(lay1, LCD_XSIZE_TFT, x1, y1,
                                lay1, LCD_XSIZE_TFT, x1, y1,
                                lay2, LCD_XSIZE_TFT, X1, i,
                                0x0c, X_W, 1);
        x1=x1 + X_W;

        if(x1>LCD_XSIZE_TFT)
        {
            x1 = x1-LCD_XSIZE_TFT;
            y1 ++;
        }
    }
}

void LT768_Print_Outside_Font_GB2312_48_72_Nand
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint32_t FlashAddr // font source address(Flash)
    , uint32_t MemoryAddr1 // cache layer address(sdram)
    , uint32_t MemoryAddr2 // cache layer address(sdram)
    , uint32_t ShowAddr // show layer address
    , uint16_t width // show layer width
    , uint8_t Size // set word size   48:48*48     72:72*72
    , uint8_t ChromaKey // 0:font background color transparent    1:set font background color
    , uint16_t x // font start x
    , uint16_t y // font start y
    , uint32_t FontColor // font color
    , uint32_t BackGroundColor // font background color((when font background is transparent, it is invalid))
    , uint16_t w // font thinkness: 0: not overstriking 1: 1 overstriking 2: 2 overstriking
    , uint16_t s // linewidth
    , uint8_t *c // data start address
)
{
    uint16_t temp_H = 0;
    uint16_t temp_L = 0;
    uint16_t temp = 0;
    uint32_t i = 0;
    uint16_t j = 0;
    uint16_t k = 0;
    uint16_t h = 0;
    uint16_t n = 0;
    uint16_t m = 0;
    uint16_t g = 0;
    uint16_t f = 0;
    h = x;
    k = y;
    Memory_8bpp_Mode(); //save picture with 8bit color depth
    Canvas_Image_Start_address(MemoryAddr1);
    Canvas_image_width(width);

    while((c[i] != '\0') && (c[i] >= 0xa0)) // if not chinese code, exit
    {
        temp_H = (c[i] - 0xa1) * 94;
        temp_L = c[i + 1] - 0xa1;
        temp = temp_H + temp_L;
        LT768_Nand_8bpp_font(1, 0, Size * j / 8, 0, Size / 8, Size,
                             FlashAddr + temp * ((Size * Size) / 8), MemoryAddr1, MemoryAddr2);
        i += 2;
        j ++;
    }

    Memory_16bpp_Mode();
    Canvas_Image_Start_address(ShowAddr);
    Canvas_image_width(width);
    j = 0;
    i = 0;

    if(w > 2)
    {
        w = 2;
    }

    for(g=0; g<w + 1; g ++)
    {
        while((c[i] != '\0') && (c[i] >= 0xa0)) // if not chinese code, exit
        {
            if((f == m) && ((x + Size*j + Size)>(width*(n + 1))))
            {
                m ++;
                n ++;
                y=y + Size-1 + s;
                x = x + ((width*n)-(x + Size*j)) + g;
                f=n;
            }

            if(ChromaKey == 0)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_8(
                    MemoryAddr2, Size * j / 8,
                    ShowAddr, width, x + Size * j, y,
                    Size, Size, FontColor, BackGroundColor
                );
            }

            if(ChromaKey == 1)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_Chroma_key_8(
                    MemoryAddr2, Size * j / 8,
                    ShowAddr, width, x + Size * j, y,
                    Size, Size, FontColor
                );
            }

            i += 2;
            j ++;
        }

        i = 0;
        j = 0;
        m = 0;
        n = 0;
        f = 0;
        x = h + g + 1;
        y = k + g + 1;
    }
}

void LT768_Print_Outside_Font_GBK_48_72_Nand
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint32_t FlashAddr // font source address(Flash)
    , uint32_t MemoryAddr1 // cache layer address(sdram)
    , uint32_t MemoryAddr2 // cache layer address(sdram)
    , uint32_t ShowAddr // show layer address
    , uint16_t width // show layer width
    , uint8_t Size // set word size   48:48*48     72:72*72
    , uint8_t ChromaKey // 0:font background color transparent    1:set font background color
    , uint16_t x // font start x
    , uint16_t y // font start y
    , uint32_t FontColor // font color
    , uint32_t BackGroundColor // font background color((when font background is transparent, it is invalid))
    , uint16_t w // font thinkness: 0: not overstriking 1: 1 overstriking 2: 2 overstriking
    , uint16_t s // linewidth
    , uint8_t *c // data start address
)
{
    uint16_t temp_H = 0;
    uint16_t temp_L = 0;
    uint16_t temp = 0;
    uint32_t i = 0;
    uint16_t j = 0;
    uint16_t k = 0;
    uint16_t h = 0;
    uint16_t n = 0;
    uint16_t m = 0;
    uint16_t g = 0;
    uint16_t f = 0;
    h = x;
    k = y;
    Memory_8bpp_Mode(); //save picture with 8bit color depth
    Canvas_Image_Start_address(MemoryAddr1);
    Canvas_image_width(width);

    while((c[i] != '\0') && (c[i] >= 0xa0)) // if not chinese code, exit
    {
        temp_H = (c[i] - 0x81) * 191;
        temp_L = c[i + 1] - 0x40;
        temp = temp_H + temp_L;
        LT768_Nand_8bpp_font(1, 0, Size * j / 8, 0, Size / 8, Size,
                             FlashAddr + temp * ((Size * Size)/8),
                             MemoryAddr1, MemoryAddr2);
        i += 2;
        j ++;
    }

    Memory_16bpp_Mode();
    Canvas_Image_Start_address(ShowAddr);
    Canvas_image_width(width);
    j = 0;
    i = 0;

    if(w > 2)
    {
        w = 2;
    }

    for(g = 0; g < w + 1; g ++)
    {
        while((c[i] != '\0') && (c[i] >= 0xa0)) // if not chinese code, exit
        {
            if((f == m) && ((x + Size * j +  Size) > (width * (n + 1))))
            {
                m ++;
                n ++;
                y = y + Size - 1 + s;
                x = x + ((width * n) - (x + Size * j)) + g;
                f = n;
            }

            if(ChromaKey == 0)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_8(
                    MemoryAddr2, Size * j / 8,
                    ShowAddr, width, x + Size * j, y,
                    Size, Size, FontColor, BackGroundColor
                );
            }

            if(ChromaKey == 1)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_Chroma_key_8(
                    MemoryAddr2, Size * j / 8,
                    ShowAddr, width, x + Size * j, y,
                    Size, Size, FontColor);
            }

            i += 2;
            j ++;
        }

        i = 0;
        j = 0;
        m = 0;
        n = 0;
        f = 0;
        x = h + g + 1;
        y = k + g + 1;
    }
}

void LT768_Print_Outside_UserDefineFont_GB2312_Nand
(
    uint8_t SCS // select SPI: 0 1
    , uint8_t Clk // SPI clock frequence divison : SPI Clock = System Clock /{(Clk + 1)*2}
    , uint32_t FlashAddr // font source address(Flash)
    , uint32_t MemoryAddr1 // cache layer address(sdram)
    , uint32_t MemoryAddr2 // cache layer address(sdram)
    , uint32_t ShowAddr // show layer address
    , uint16_t width // show layer width
    , uint8_t Size // set word size   48:48*48     72:72*72
    , uint8_t ChromaKey // 0:font background color transparent    1:set font background color
    , uint16_t x // font start x
    , uint16_t y // font start y
    , uint32_t FontColor // font color
    , uint32_t BackGroundColor // font background color((when font background is transparent, it is invalid))
    , uint16_t w // font thinkness: 0: not overstriking 1: 1 overstriking 2: 2 overstriking
    , uint16_t s // linewidth
    , uint8_t *c // data start address
)
{
    short temp = 0;
    uint32_t i = 0;
    uint16_t j = 0;
    uint16_t k = 0;
    uint16_t h = 0;
    uint16_t n = 0;
    uint16_t m = 0;
    uint16_t g = 0;
    uint16_t f = 0;
    uint8_t temp1[2] = {0};
    uint32_t fontNum = 0;
    clear_Font_1();
    LT768_SPI_Init(SCS, 1);
    LT_W25N01GV_Read(temp1, FlashAddr + 8, 2);
    fontNum = (temp1[0] << 8) + temp1[1];
    LT_W25N01GV_Read(Font_1, FlashAddr + 0xc, fontNum * 2);
    h = x;
    k = y;
    Memory_8bpp_Mode();//save picture with 8bit color depth
    Canvas_Image_Start_address(MemoryAddr1);
    Canvas_image_width(width);

    while(c[i + 1] != '\0')
    {
        temp = Get_User_Font_P(c[i], c[i + 1]);

        if(temp != -1)
        {
            LT768_Nand_8bpp_font(SCS, Clk, Size * j / 8, 0, Size / 8, Size,
                                 FlashAddr + temp * ((Size * Size) / 8) + 0x0c + fontNum * 2,
                                 MemoryAddr1, MemoryAddr2);
        }
        else
        {
            break;
        }

        i += 2;
        j ++;
    }

    Memory_16bpp_Mode();
    Canvas_Image_Start_address(ShowAddr);
    Canvas_image_width(width);
    j = 0;
    i = 0;

    if(w > 2)
    {
        w = 2;
    }

    for(g = 0; g < w + 1; g ++)
    {
        while(c[i + 1] != '\0')
        {
            temp = Get_User_Font_P(c[i], c[i + 1]);

            if(temp == -1)
            {
                break;
            }

            if((f == m) && ((x + Size * j + Size) > (width * (n + 1))))
            {
                m ++;
                n ++;
                y = y + Size - 1 + s;
                x = x + ((width * n) - (x + Size * j)) + g;
                f = n;
            }

            if(ChromaKey == 1)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_8(
                    MemoryAddr2, Size * j / 8,
                    ShowAddr, width, x + Size * j, y,
                    Size, Size, FontColor, BackGroundColor
                );
            }

            if(ChromaKey == 0)
            {
                LT768_BTE_Memory_Copy_ColorExpansion_Chroma_key_8(
                    MemoryAddr2, Size * j / 8,
                    ShowAddr, width, x + Size*j, y,
                    Size, Size, FontColor
                );
            }

            i += 2;
            j ++;
        }

        ChromaKey = 0;
        i = 0;
        j = 0;
        m = 0;
        n = 0;
        f = 0;
        x = h + g + 1;
        y = k + g + 1;
    }
}

