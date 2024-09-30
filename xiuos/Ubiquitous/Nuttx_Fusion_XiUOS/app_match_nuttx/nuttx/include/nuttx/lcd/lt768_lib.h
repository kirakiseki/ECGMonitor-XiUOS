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
 * @file lt768_lib.h
 * @brief lt768 register relative driver, inherit from Levetop Electronics
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.9.19
 */

#ifndef __LT768_LIB_H_
#define __LT768_LIB_H_
#include "lt768.h"

//external OSC
#define XI_4M            0
#define XI_8M            1
#define XI_10M           0
#define XI_12M           0

//resolution
#define LCD_XSIZE_TFT  480
#define LCD_YSIZE_TFT  272

//parameter
#define LCD_VBPD    20
#define LCD_VFPD    12
#define LCD_VSPW    3
#define LCD_HBPD    140
#define LCD_HFPD    160
#define LCD_HSPW    20

#define color256_black   0x00
#define color256_white   0xff
#define color256_red     0xe0
#define color256_green   0x1c
#define color256_blue    0x03
#define color256_yellow  color256_red|color256_green
#define color256_cyan    color256_green|color256_blue
#define color256_purple  color256_red|color256_blue

#define color65k_black   0x0000
#define color65k_white   0xffff
#define color65k_red     0xf800
#define color65k_green   0x07e0
#define color65k_blue    0x001f
#define color65k_yellow  color65k_red|color65k_green
#define color65k_cyan    color65k_green|color65k_blue
#define color65k_purple  color65k_red|color65k_blue

#define color65k_grayscale1    2113
#define color65k_grayscale2    2113*2
#define color65k_grayscale3    2113*3
#define color65k_grayscale4    2113*4
#define color65k_grayscale5    2113*5
#define color65k_grayscale6    2113*6
#define color65k_grayscale7    2113*7
#define color65k_grayscale8    2113*8
#define color65k_grayscale9    2113*9
#define color65k_grayscale10   2113*10
#define color65k_grayscale11   2113*11
#define color65k_grayscale12   2113*12
#define color65k_grayscale13   2113*13
#define color65k_grayscale14   2113*14
#define color65k_grayscale15   2113*15
#define color65k_grayscale16   2113*16
#define color65k_grayscale17   2113*17
#define color65k_grayscale18   2113*18
#define color65k_grayscale19   2113*19
#define color65k_grayscale20   2113*20
#define color65k_grayscale21   2113*21
#define color65k_grayscale22   2113*22
#define color65k_grayscale23   2113*23
#define color65k_grayscale24   2113*24
#define color65k_grayscale25   2113*25
#define color65k_grayscale26   2113*26
#define color65k_grayscale27   2113*27
#define color65k_grayscale28   2113*28
#define color65k_grayscale29   2113*29
#define color65k_grayscale30   2113*30

#define color16M_black   0x00000000
#define color16M_white   0x00ffffff
#define color16M_red     0x00ff0000
#define color16M_green   0x0000ff00
#define color16M_blue    0x000000ff
#define color16M_yellow  color16M_red|color16M_green
#define color16M_cyan    color16M_green|color16M_blue
#define color16M_purple  color16M_red|color16M_blue

/* LCD color */
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000  
#define BLUE         	 0x001F
#define GREY             0xF7DE  
#define BRED             0xF81F
#define GRED 			 0xFFE0
#define GBLUE			 0x07FF
#define BLUE2            0x051F 
#define RED           	 0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN 		     0xBC40    
#define BRRED 	   		 0xFC07      
#define GRAY             0x8430      

#define DARKBLUE      	 0x01CF
#define LIGHTBLUE      	 0x7D7C
#define GRAYBLUE         0x5458
#define LIGHTGREEN       0x841F 
#define LGRAY 	         0xC618
#define LGRAYBLUE        0xA651
#define LBBLUE           0x2B12


#define Line0          0
#define Line1          24
#define Line2          48
#define Line3          72
#define Line4          96
#define Line5          120
#define Line6          144
#define Line7          168
#define Line8          192
#define Line9          216
#define Line10         240
#define Line11         264
#define Line12         288
#define Line13         312
#define Line14         336
#define Line15         360
#define Line16         384
#define Line17         408
#define Line18         432
#define Line19         456
#define Line20         480
#define Line21         504
#define Line22         528
#define Line23         552
#define Line24         576
#define Line25         600

void lt768_init(void);

/* write to memory */
void MPU8_8bpp_Memory_Write(uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h,
    const uint8_t *data);
void MPU8_16bpp_Memory_Write(uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h,
    const uint8_t *data);
void MPU8_24bpp_Memory_Write(uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h,
    const uint8_t *data);
void MPU16_16bpp_Memory_Write(uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h,
    const uint16_t *data);
void MPU16_24bpp_Mode1_Memory_Write(uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h,
    const uint16_t *data);
void MPU16_24bpp_Mode2_Memory_Write(uint16_t x,
    uint16_t y,
    uint16_t w,
    uint16_t h,
    const uint16_t *data);

/* draw line */
void LT768_DrawLine(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint32_t LineColor);
void LT768_DrawLine_Width(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint32_t LineColor,
    uint16_t Width);

/* draw circle */
void LT768_DrawCircle(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t R,
    uint32_t CircleColor);
void LT768_DrawCircle_Fill(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t R,
    uint32_t ForegroundColor);
void LT768_DrawCircle_Width(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t R,
    uint32_t CircleColor,
    uint32_t ForegroundColor,
    uint16_t Width);

/* draw elipse */
void LT768_DrawEllipse(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t EllipseColor);
void LT768_DrawEllipse_Fill(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t ForegroundColor);
void LT768_DrawEllipse_Width(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t EllipseColor,
    uint32_t ForegroundColor,
    uint16_t Width);

/* draw square */
void LT768_DrawSquare(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint32_t SquareColor);
void LT768_DrawSquare_Fill(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint32_t ForegroundColor);
void LT768_DrawSquare_Width(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint32_t SquareColor,
    uint32_t ForegroundColor,
    uint16_t Width);

/* draw circle square  */
void LT768_DrawCircleSquare(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t CircleSquareColor);
void LT768_DrawCircleSquare_Fill(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t ForegroundColor);
void LT768_DrawCircleSquare_Width(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t CircleSquareColor,
    uint32_t ForegroundColor,
    uint16_t Width);

/* draw triangle */
void LT768_DrawTriangle(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X3,
    uint16_t Y3,
    uint32_t TriangleColor);
void LT768_DrawTriangle_Fill(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X3,
    uint16_t Y3,
    uint32_t ForegroundColor);
void LT768_DrawTriangle_Frame(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X3,
    uint16_t Y3,
    uint32_t TriangleColor,
    uint32_t ForegroundColor);

/* draw quadrilateral */
void LT768_DrawQuadrilateral(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X3,
    uint16_t Y3,
    uint16_t X4,
    uint16_t Y4,
    uint32_t ForegroundColor);
void LT768_DrawQuadrilateral_Fill(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X3,
    uint16_t Y3,
    uint16_t X4,
    uint16_t Y4,
    uint32_t ForegroundColor);
void LT768_DrawTriangle_Frame(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X3,
    uint16_t Y3,
    uint32_t TriangleColor,
    uint32_t ForegroundColor);

/* draw pentagon */
void LT768_DrawPentagon(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X3,
    uint16_t Y3,
    uint16_t X4,
    uint16_t Y4,
    uint16_t X5,
    uint16_t Y5,
    uint32_t ForegroundColor);
void LT768_DrawPentagon_Fill(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X3,
    uint16_t Y3,
    uint16_t X4,
    uint16_t Y4,
    uint16_t X5,
    uint16_t Y5,
    uint32_t ForegroundColor);

/* draw curve */
void LT768_DrawLeftUpCurve(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t CurveColor);
void LT768_DrawLeftDownCurve(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t CurveColor);
void LT768_DrawRightUpCurve(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t CurveColor);
void LT768_DrawRightDownCurve(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t CurveColor);
void LT768_SelectDrawCurve(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t CurveColor,
    uint16_t  Dir);

/* draw 1/4 filled curve */
void LT768_DrawLeftUpCurve_Fill(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t ForegroundColor);
void LT768_DrawLeftDownCurve_Fill(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t ForegroundColor);
void LT768_DrawRightUpCurve_Fill(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t ForegroundColor);
void LT768_DrawRightDownCurve_Fill(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t ForegroundColor);
void LT768_SelectDrawCurve_Fill(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint32_t CurveColor,
    uint16_t  Dir);

/* draw cylinders */
uint8_t LT768_DrawCylinder(uint16_t XCenter,
    uint16_t YCenter,
    uint16_t X_R,
    uint16_t Y_R,
    uint16_t H,
    uint32_t CylinderColor,
    uint32_t ForegroundColor);

/* draw quadrangular */
void LT768_DrawQuadrangular(uint16_t X1,
    uint16_t Y1,
    uint16_t X2,
    uint16_t Y2,
    uint16_t X3,
    uint16_t Y3,
    uint16_t X4,
    uint16_t Y4,
    uint16_t X5,
    uint16_t Y5,
    uint16_t X6,
    uint16_t Y6,
    uint32_t QuadrangularColor,
    uint32_t ForegroundColor);

/* table */
void LT768_MakeTable(uint16_t X1,
    uint16_t Y1,
    uint16_t W,
    uint16_t H,
    uint16_t Line,
    uint16_t Row,
    uint32_t  TableColor,
    uint32_t  ItemColor,
    uint32_t  ForegroundColor,
    uint16_t width1,
    uint16_t width2,
    uint8_t  mode);

/* linear DMA transport to SDRAM */
void LT768_DMA_24bit_Linear(uint8_t SCS,
    uint8_t Clk,
    uint32_t flash_addr,
    uint32_t memory_ad,
    uint32_t data_num);
void LT768_DMA_32bit_Linear(uint8_t SCS,
    uint8_t Clk,
    uint32_t flash_addr,
    uint32_t memory_ad,
    uint32_t data_num);

/* DMA transport to SDRAM */
void LT768_DMA_24bit_Block(uint8_t SCS,
    uint8_t Clk,
    uint16_t X1,
    uint16_t Y1,
    uint16_t X_W,
    uint16_t Y_H,
    uint16_t P_W,
    uint32_t Addr);
void LT768_DMA_32bit_Block(uint8_t SCS,
    uint8_t Clk,
    uint16_t X1,
    uint16_t Y1,
    uint16_t X_W,
    uint16_t Y_H,
    uint16_t P_W,
    uint32_t Addr);

/* use internal font */
void LT768_Select_Internal_Font_Init(uint8_t Size,
    uint8_t XxN,
    uint8_t YxN,
    uint8_t ChromaKey,
    uint8_t Alignment);
void LT768_Print_Internal_Font_String(uint16_t x,
    uint16_t y,
    uint32_t FontColor,
    uint32_t BackGroundColor,
    uint8_t *c);

/* nor flash use outside font */
/* 16*16 24*24 32*32 */
void LT768_Select_Outside_Font_Init(uint8_t SCS,
    uint8_t Clk,
    uint32_t FlashAddr,
    uint32_t MemoryAddr,
    uint32_t Num,
    uint8_t Size,
    uint8_t XxN,
    uint8_t YxN,
    uint8_t ChromaKey,
    uint8_t Alignment);
void LT768_Print_Outside_Font_String(uint16_t x,
    uint16_t y,
    uint32_t FontColor,
    uint32_t BackGroundColor,
    uint8_t *c);
void LT768_Print_Outside_Font_GBK_String(uint16_t x,
    uint16_t y,
    uint32_t FontColor,
    uint32_t BackGroundColor,
    uint8_t *c);
/* 48*48 72*72 SBC case */
void LT768_BTE_Memory_Copy_ColorExpansion_8(uint32_t S0_Addr,
    uint16_t YS0,
    uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    uint16_t X_W,
    uint16_t Y_H,
    uint32_t Foreground_color,
    uint32_t Background_color);
void LT768_BTE_Memory_Copy_ColorExpansion_Chroma_key_8(uint32_t S0_Addr,
    uint16_t YS0,
    uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    uint16_t X_W,
    uint16_t Y_H,
    uint32_t Foreground_color);
void LT768_Print_Outside_Font_GB2312_48_72(uint8_t SCS,
    uint8_t Clk,
    uint32_t FlashAddr,
    uint32_t MemoryAddr,
    uint32_t ShowAddr,
    uint16_t width,
    uint8_t Size,
    uint8_t ChromaKey,
    uint16_t x,
    uint16_t y,
    uint32_t FontColor,
    uint32_t BackGroundColor,
    uint16_t w,
    uint16_t s,
    uint8_t *c);
void LT768_Print_Outside_Font_BIG5_48_72(uint8_t SCS,
    uint8_t Clk,
    uint32_t FlashAddr,
    uint32_t MemoryAddr,
    uint32_t ShowAddr,
    uint16_t width,
    uint8_t Size,
    uint8_t ChromaKey,
    uint16_t x,
    uint16_t y,
    uint32_t FontColor,
    uint32_t BackGroundColor,
    uint16_t w,
    uint16_t s,
    uint8_t *c);
void LT768_Print_Outside_Font_GBK_48_72(uint8_t SCS,
    uint8_t Clk,
    uint32_t FlashAddr,
    uint32_t MemoryAddr,
    uint32_t ShowAddr,
    uint16_t width,
    uint8_t Size,
    uint8_t ChromaKey,
    uint16_t x,
    uint16_t y,
    uint32_t FontColor,
    uint32_t BackGroundColor,
    uint16_t w,
    uint16_t s,
    uint8_t *c);

/* nor flash used define font */
/* 16*16 24*24 32*32 48*48 72*72 SBC case */
int Get_User_Font_P(char chH,
    char chL);
void LT768_Print_Outside_UserDefineFont_GB2312(uint8_t SCS,
    uint8_t Clk,
    uint32_t FlashAddr,
    uint32_t MemoryAddr,
    uint32_t ShowAddr,
    uint16_t width,
    uint8_t Size,
    uint8_t ChromaKey,
    uint16_t x,
    uint16_t y,
    uint32_t FontColor,
    uint32_t BackGroundColor,
    uint16_t w,
    uint16_t s,
    uint8_t *c);

/* text cursor */
void LT768_Text_cursor_Init(uint8_t On_Off_Blinking,
    uint16_t Blinking_Time,
    uint16_t X_W,
    uint16_t Y_W);
void LT768_Enable_Text_Cursor(void);
void LT768_Disable_Text_Cursor(void);

/* graphic cursor */
void LT768_Graphic_cursor_Init(uint8_t Cursor_N,
    uint8_t Color1,
    uint8_t Color2,
    uint16_t X_Pos,
    uint16_t Y_Pos,
    uint8_t *Cursor_Buf);
void LT768_Set_Graphic_cursor_Pos(uint8_t Cursor_N,
    uint16_t X_Pos,
    uint16_t Y_Pos);
void LT768_Enable_Graphic_Cursor(void);
void LT768_Disable_Graphic_Cursor(void);

/* block transport enginer (BitBLT) */
void BTE_Solid_Fill(uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    uint16_t color,
    uint16_t X_W,
    uint16_t Y_H);
void LT768_BTE_Memory_Copy(uint32_t S0_Addr,
    uint16_t S0_W,
    uint16_t XS0,
    uint16_t YS0,
    uint32_t S1_Addr,
    uint16_t S1_W,
    uint16_t XS1,
    uint16_t YS1,
    uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    unsigned int ROP_Code,
    uint16_t X_W,
    uint16_t Y_H);
void LT768_BTE_Memory_Copy_Chroma_key(uint32_t S0_Addr,
    uint16_t S0_W,
    uint16_t XS0,
    uint16_t YS0,
    uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    uint32_t Background_color,
    uint16_t X_W,
    uint16_t Y_H);
void LT768_BTE_Pattern_Fill(uint8_t P_8x8_or_16x16,
    uint32_t S0_Addr,
    uint16_t S0_W,
    uint16_t XS0,
    uint16_t YS0,
    uint32_t Des_Addr,
    uint16_t Des_W,
     uint16_t XDes,
    uint16_t YDes,
    unsigned int ROP_Code,
    uint16_t X_W,
    uint16_t Y_H);
void LT768_BTE_Pattern_Fill_With_Chroma_key(uint8_t P_8x8_or_16x16,
    uint32_t S0_Addr,
    uint16_t S0_W,
    uint16_t XS0,
    uint16_t YS0,
    uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    unsigned int ROP_Code,
    uint32_t Background_color,
    uint16_t X_W,
    uint16_t Y_H);
void LT768_BTE_MCU_Write_MCU_16bit(uint32_t S1_Addr,
    uint16_t S1_W,
    uint16_t XS,
    uint16_t YS1,
    uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    unsigned int ROP_Code,
    uint16_t X_W,
    uint16_t Y_H,
    const uint16_t *data);
void LT768_BTE_MCU_Write_Chroma_key_MCU_16bit(uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    uint32_t Background_color,
    uint16_t X_W,
    uint16_t Y_H,
    const uint16_t *data);
void LT768_BTE_MCU_Write_ColorExpansion_MCU_16bit(uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    uint16_t X_W,
    uint16_t Y_H,
    uint32_t Foreground_color,
    uint32_t Background_color,
    const uint16_t *data);
void LT768_BTE_MCU_Write_ColorExpansion_Chroma_key_MCU_16bit(uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    uint16_t X_W,
    uint16_t Y_H,
    uint32_t Foreground_color,
    const uint16_t *data);
void BTE_Alpha_Blending(uint32_t S0_Addr,
    uint16_t S0_W,
    uint16_t XS0,
    uint16_t YS0,
    uint32_t S1_Addr,
    uint16_t S1_W,
    uint16_t XS1,
    uint16_t YS1,
    uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    uint16_t X_W,
    uint16_t Y_H,
    uint8_t alpha);
void BTE_Pixel_8bpp_Alpha_Blending(uint32_t S0_Addr,
    uint16_t S0_W,
    uint16_t XS0,
    uint16_t YS0,
    uint32_t S1_Addr,
    uint16_t S1_W,
    uint16_t XS1,
    uint16_t YS1,
    uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    uint16_t X_W,
    uint16_t Y_H);
void BTE_Pixel_16bpp_Alpha_Blending(uint32_t S0_Addr,
    uint16_t S0_W,
    uint16_t XS0,
    uint16_t YS0,
    uint32_t S1_Addr,
    uint16_t S1_W,
    uint16_t XS1,
    uint16_t YS1,
    uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    uint16_t X_W,
    uint16_t Y_H);

/*  PIP */
void LT768_PIP_Init(uint8_t On_Off,
    uint8_t Select_PIP,
    uint32_t PAddr,
    uint16_t XP,
    uint16_t YP,
    uint32_t ImageWidth,
    uint16_t X_Dis,
    uint16_t Y_Dis,
    uint16_t X_W,
    uint16_t Y_H);
void LT768_Set_DisWindowPos(uint8_t On_Off,
    uint8_t Select_PIP,
    uint16_t X_Dis,
    uint16_t Y_Dis);

/*  PWM */
void LT768_PWM0_Init(uint8_t on_off,
    uint8_t Clock_Divided,
    uint8_t Prescalar,
    uint16_t Count_Buffer,
    uint16_t Compare_Buffer);
void LT768_PWM1_Init(uint8_t on_off,
    uint8_t Clock_Divided,
    uint8_t Prescalar,
    uint16_t Count_Buffer,
    uint16_t Compare_Buffer);
void LT768_PWM0_Duty(uint16_t Compare_Buffer);
void LT768_PWM1_Duty(uint16_t Compare_Buffer);

/*  Standby Mode */
void LT768_Standby(void);
void LT768_Wkup_Standby(void);

/* Suspend Mode */
void LT768_Suspend(void);
void LT768_Wkup_Suspend(void);

/* Sleep Mode */
void LT768_SleepMode(void);
void LT768_Wkup_Sleep(void);

/* W25QXX */
void LT768_SPI_Init(uint8_t cs,
    uint8_t div);
void W25QXX_Enter_4Byte_AddressMode(void);
void LT_W25QXX_Read(uint8_t *pBuffer,
    uint32_t ReadAddr,
    uint16_t NumByteToRead);

/* nand flash W25N01GV */
uint8_t W25N01GV_ReadSR(uint8_t reg);
void W25N01GV_Write_SR(uint8_t reg,
    uint8_t val);
void W25N01GV_Wait_Busy(void);
void W25N01GV_ContinuousRead_Mode(void);
void W25N01GV_Write_Page(uint16_t page);
void W25N01GV_ReadPageAddr_Data(uint8_t *pBuffer,
    uint32_t PageNum,
    uint32_t PageAddr,
    uint16_t NumByteToRead);
void LT_W25N01GV_Read(uint8_t *pBuffer,
    uint32_t ReadAddr,
    uint16_t NumByteToRead);

/* nand flash draw picture */
void LT768_Nand_Pic(uint8_t SCS,
    uint16_t X1,
    uint16_t Y1,
    uint16_t X_W,
    uint16_t Y_H,
    uint32_t Addr,
    uint32_t lay0,
    uint32_t lay1);

/* nand flash use outside font */
/* 16*16 24*24 32*32 */
void NandFlash_Select_Outside_Font_Init(uint8_t SCS,
    uint8_t Clk,
    uint32_t FlashAddr,
    uint32_t MemoryAddr,
    uint32_t Num,
    uint8_t Size,
    uint8_t XxN,
    uint8_t YxN,
    uint8_t ChromaKey,
    uint8_t Alignment);

/* nand flash use outside font */
/* 48*48 72*72 SBC case */
void LT768_BTE_Memory_Copy_8(uint32_t S0_Addr,
    uint16_t S0_W,
    uint16_t XS0,
    uint16_t YS0,
    uint32_t S1_Addr,
    uint16_t S1_W,
    uint16_t XS1,
    uint16_t YS1,
    uint32_t Des_Addr,
    uint16_t Des_W,
    uint16_t XDes,
    uint16_t YDes,
    unsigned int ROP_Code,
    uint16_t X_W,
    uint16_t Y_H);
void LT768_Nand_8bpp_font(uint8_t SCS,
    uint8_t Clk,
    uint16_t X1,
    uint16_t Y1,
    uint16_t X_W,
    uint16_t Y_H,
    uint32_t Addr,
    uint32_t lay1,
    uint32_t lay2);
void LT768_Print_Outside_Font_GB2312_48_72_Nand(uint8_t SCS,
    uint8_t Clk,
    uint32_t FlashAddr,
    uint32_t MemoryAddr1,
    uint32_t MemoryAddr2,
    uint32_t ShowAddr,
    uint16_t width,
    uint8_t Size,
    uint8_t ChromaKey,
    uint16_t x,
    uint16_t y,
    uint32_t FontColor,
    uint32_t BackGroundColor,
    uint16_t w,
    uint16_t s,
    uint8_t *c);
void LT768_Print_Outside_Font_GBK_48_72_Nand(uint8_t SCS,
    uint8_t Clk,
    uint32_t FlashAddr,
    uint32_t MemoryAddr1,
    uint32_t MemoryAddr2,
    uint32_t ShowAddr,
    uint16_t width,
    uint8_t Size,
    uint8_t ChromaKey,
    uint16_t x,
    uint16_t y,
    uint32_t FontColor,
    uint32_t BackGroundColor,
    uint16_t w,
    uint16_t s,
    uint8_t *c);

/* nand flash user defined font */
/* 16*16 24*24 32*32 48*48 72*72 SBC case */
void LT768_Print_Outside_UserDefineFont_GB2312_Nand(uint8_t SCS,
    uint8_t Clk,
    uint32_t FlashAddr,
    uint32_t MemoryAddr1,
    uint32_t MemoryAddr2,
    uint32_t ShowAddr,
    uint16_t width,
    uint8_t Size,
    uint8_t ChromaKey,
    uint16_t x,
    uint16_t y,
    uint32_t FontColor,
    uint32_t BackGroundColor,
    uint16_t w,
    uint16_t s,
    uint8_t *c);
#endif
