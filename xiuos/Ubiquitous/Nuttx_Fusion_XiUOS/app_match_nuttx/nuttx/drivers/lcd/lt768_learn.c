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
 * @file lt768_learn.c
 * @brief lt768_learn.c
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2022.9.19
 */
#include <syslog.h>
#include "nuttx/arch.h"
#include "nuttx/lcd/lt768_learn.h"

const uint8_t g_Image_pen_il[256] = { /* 0X00,0X02,0X20,0X00,0X20,0X00, */
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0X96,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X91,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0XA4,0X15,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XA4,0X00,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,
0XA9,0X01,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XA9,0X00,0X46,0XAA,0XAA,0XAA,0XAA,0XAA,
0XAA,0X40,0X51,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X14,0X6A,0XAA,0XAA,0XAA,0XAA,
0XAA,0XA4,0X05,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XA9,0X01,0X46,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0X40,0X51,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X14,0X6A,0XAA,0XAA,0XAA,
0XAA,0XAA,0XA4,0X05,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XA9,0X01,0X46,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0X40,0X51,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X14,0X69,0XAA,0XAA,
0XAA,0XAA,0XAA,0XA4,0X01,0X14,0X6A,0XAA,0XAA,0XAA,0XAA,0XA9,0X00,0X44,0X1A,0XAA,
0XAA,0XAA,0XAA,0XAA,0X40,0X11,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X04,0X41,0XAA,
0XAA,0XAA,0XAA,0XAA,0XA4,0X01,0X10,0X6A,0XAA,0XAA,0XAA,0XAA,0XA9,0X00,0X44,0X1A,
0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X11,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X04,0X1A,
0XAA,0XAA,0XAA,0XAA,0XAA,0XA4,0X01,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XA9,0X00,0X1A,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X95,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
};

const uint8_t g_Image_arrow_il[256] = { /* 0X00,0X02,0X20,0X00,0X20,0X00, */
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0X5A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X46,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0X41,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0X40,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0X40,0X01,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X00,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,
0X40,0X00,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X00,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,
0X40,0X00,0X01,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X00,0X00,0X6A,0XAA,0XAA,0XAA,0XAA,
0X40,0X00,0X00,0X1A,0XAA,0XAA,0XAA,0XAA,0X40,0X00,0X00,0X06,0XAA,0XAA,0XAA,0XAA,
0X40,0X00,0X00,0X01,0XAA,0XAA,0XAA,0XAA,0X40,0X00,0X00,0X00,0X6A,0XAA,0XAA,0XAA,
0X40,0X00,0X15,0X55,0X5A,0XAA,0XAA,0XAA,0X40,0X10,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,
0X40,0X64,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,0X41,0XA4,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,
0X46,0XA9,0X01,0XAA,0XAA,0XAA,0XAA,0XAA,0X5A,0XA9,0X01,0XAA,0XAA,0XAA,0XAA,0XAA,
0X6A,0XAA,0X40,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X40,0X6A,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0X90,0X1A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X90,0X1A,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XA4,0X06,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XA4,0X06,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XA9,0X5A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
};

const uint8_t g_Image_busy_im[256] = { /* 0X00,0X02,0X20,0X00,0X20,0X00, */
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X55,0X55,0X55,0X55,0X6A,0XAA,
0XAA,0XAA,0X54,0X00,0X00,0X05,0X6A,0XAA,0XAA,0XAA,0X55,0X55,0X55,0X55,0X6A,0XAA,
0XAA,0XAA,0X94,0X00,0X00,0X05,0XAA,0XAA,0XAA,0XAA,0X94,0X00,0X00,0X05,0XAA,0XAA,
0XAA,0XAA,0X94,0X44,0X44,0X45,0XAA,0XAA,0XAA,0XAA,0X94,0X11,0X11,0X05,0XAA,0XAA,
0XAA,0XAA,0X95,0X04,0X44,0X15,0XAA,0XAA,0XAA,0XAA,0XA5,0X41,0X10,0X56,0XAA,0XAA,
0XAA,0XAA,0XA9,0X50,0X41,0X5A,0XAA,0XAA,0XAA,0XAA,0XAA,0X54,0X05,0X6A,0XAA,0XAA,
0XAA,0XAA,0XAA,0X94,0X05,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X94,0X05,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0X94,0X45,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X94,0X05,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0X50,0X01,0X6A,0XAA,0XAA,0XAA,0XAA,0XA9,0X40,0X40,0X5A,0XAA,0XAA,
0XAA,0XAA,0XA5,0X00,0X10,0X16,0XAA,0XAA,0XAA,0XAA,0X94,0X00,0X00,0X05,0XAA,0XAA,
0XAA,0XAA,0X94,0X04,0X44,0X05,0XAA,0XAA,0XAA,0XAA,0X94,0X11,0X11,0X05,0XAA,0XAA,
0XAA,0XAA,0X94,0X44,0X44,0X45,0XAA,0XAA,0XAA,0XAA,0X95,0X11,0X11,0X15,0XAA,0XAA,
0XAA,0XAA,0X55,0X55,0X55,0X55,0X6A,0XAA,0XAA,0XAA,0X54,0X00,0X00,0X05,0X6A,0XAA,
0XAA,0XAA,0X55,0X55,0X55,0X55,0X6A,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
};

const uint8_t g_Image_no_im[256] = { /* 0X00,0X02,0X20,0X00,0X20,0X00, */
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0X95,0X55,0XAA,0XAA,0XAA,
0XAA,0XAA,0XA9,0X40,0X00,0X5A,0XAA,0XAA,0XAA,0XAA,0X94,0X00,0X00,0X05,0XAA,0XAA,
0XAA,0XAA,0X40,0X00,0X00,0X00,0X6A,0XAA,0XAA,0XA9,0X00,0X15,0X55,0X00,0X1A,0XAA,
0XAA,0XA4,0X00,0X6A,0XAA,0X50,0X06,0XAA,0XAA,0XA4,0X00,0X6A,0XAA,0XA4,0X06,0XAA,
0XAA,0X90,0X00,0X1A,0XAA,0XA9,0X01,0XAA,0XAA,0X90,0X10,0X06,0XAA,0XA9,0X01,0XAA,
0XAA,0X40,0X64,0X01,0XAA,0XAA,0X40,0X6A,0XAA,0X40,0X69,0X00,0X6A,0XAA,0X40,0X6A,
0XAA,0X40,0X6A,0X40,0X1A,0XAA,0X40,0X6A,0XAA,0X40,0X6A,0X90,0X06,0XAA,0X40,0X6A,
0XAA,0X40,0X6A,0XA4,0X01,0XAA,0X40,0X6A,0XAA,0X40,0X6A,0XA9,0X00,0X6A,0X40,0X6A,
0XAA,0X40,0X6A,0XAA,0X40,0X1A,0X40,0X6A,0XAA,0X90,0X1A,0XAA,0X90,0X05,0X01,0XAA,
0XAA,0X90,0X1A,0XAA,0XA4,0X00,0X01,0XAA,0XAA,0XA4,0X06,0XAA,0XA9,0X00,0X06,0XAA,
0XAA,0XA4,0X01,0X6A,0XAA,0X40,0X06,0XAA,0XAA,0XA9,0X00,0X15,0X55,0X00,0X1A,0XAA,
0XAA,0XAA,0X40,0X00,0X00,0X00,0X6A,0XAA,0XAA,0XAA,0X94,0X00,0X00,0X05,0XAA,0XAA,
0XAA,0XAA,0XA9,0X40,0X00,0X5A,0XAA,0XAA,0XAA,0XAA,0XAA,0X95,0X55,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,0XAA,
};


void Display_RGB(void)
{
	Select_Main_Window_16bpp();
	Main_Image_Start_Address(0);				
	Main_Image_Width(LCD_XSIZE_TFT);
	Main_Window_Start_XY(0,0);
	Canvas_Image_Start_address(0);
	Canvas_image_width(LCD_XSIZE_TFT);
  	Active_Window_XY(0,0);
	Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);
	
	while(1)
	{
		BTE_Solid_Fill(0,LCD_XSIZE_TFT,0,0,RED,LCD_XSIZE_TFT,LCD_YSIZE_TFT);
		up_mdelay(500);
		BTE_Solid_Fill(0,LCD_XSIZE_TFT,0,0,GREEN,LCD_XSIZE_TFT,LCD_YSIZE_TFT);
		up_mdelay(500);
		BTE_Solid_Fill(0,LCD_XSIZE_TFT,0,0,BLUE,LCD_XSIZE_TFT,LCD_YSIZE_TFT);
		up_mdelay(500);
	}
}

#define Picture_1_Addr 0
#define Picture_2_Addr (LCD_XSIZE_TFT*LCD_YSIZE_TFT*2)
#define Picture_3_Addr (LCD_XSIZE_TFT*LCD_YSIZE_TFT*4)
#define Picture_4_Addr (LCD_XSIZE_TFT*LCD_YSIZE_TFT*6)

void Display_Picture(void)
{
	uint8_t i = 0;
	
	Select_Main_Window_16bpp();
	Main_Image_Start_Address(0);				
	Main_Image_Width(LCD_XSIZE_TFT);
	Main_Window_Start_XY(0,0);
	Canvas_Image_Start_address(0);
	Canvas_image_width(LCD_XSIZE_TFT);
  	Active_Window_XY(0,0);
	Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);
	
	while(1)
	{
	for(i = 0;i < 4;i++)
	{
			switch(i)
			{
				case 0:LT768_DMA_24bit_Block(1,0,0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,LCD_XSIZE_TFT,Picture_1_Addr);break;
				case 1:LT768_DMA_24bit_Block(1,0,0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,LCD_XSIZE_TFT,Picture_2_Addr);break;
				case 2:LT768_DMA_24bit_Block(1,0,0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,LCD_XSIZE_TFT,Picture_3_Addr);break;
				case 3:LT768_DMA_24bit_Block(1,0,0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,LCD_XSIZE_TFT,Picture_4_Addr);break;
				default:	break;
			}
			up_mdelay(500);
		}
	}
}


void Display_PIP(void)
{
	unsigned int i;
	
	Select_Main_Window_16bpp();
	Main_Image_Start_Address(0);				
	Main_Image_Width(LCD_XSIZE_TFT);
	Main_Window_Start_XY(0,0);
	Canvas_Image_Start_address(0);
	Canvas_image_width(LCD_XSIZE_TFT);
  	Active_Window_XY(0,0);
	Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);
	
	BTE_Solid_Fill(0,LCD_XSIZE_TFT,0,0,BLUE2,LCD_XSIZE_TFT,LCD_YSIZE_TFT);                             // 底图颜色
	BTE_Solid_Fill(LCD_XSIZE_TFT*LCD_YSIZE_TFT*2,LCD_XSIZE_TFT,0,0,RED,LCD_XSIZE_TFT,LCD_YSIZE_TFT);   // PIP1颜色
	BTE_Solid_Fill(LCD_XSIZE_TFT*LCD_YSIZE_TFT*4,LCD_XSIZE_TFT,0,0,GREEN,LCD_XSIZE_TFT,LCD_YSIZE_TFT); // PIP2颜色
	
	LT768_PIP_Init(1,1,LCD_XSIZE_TFT*LCD_YSIZE_TFT*2,250,250,1024,0,175,250,250);     // 初始化PIP1           
	LT768_PIP_Init(1,2,LCD_XSIZE_TFT*LCD_YSIZE_TFT*4,300,300,1024,774,150,300,300);   // 初始化PIP2
	
	while(1)
	{
		for(i=0;i<550;i=i+2)
		{			
			LT768_Set_DisWindowPos(1,1,i,175);
			LT768_Set_DisWindowPos(1,2,734-i,150);
			up_mdelay(5);
		}

		for(i=0;i<550;i=i+2)
		{
			LT768_Set_DisWindowPos(1,1,550-i,175);
			LT768_Set_DisWindowPos(1,2,734-550+i,150);
			up_mdelay(5);
		}
	}
}

void Display_Internal_Font(void)
{
	char c[2] = "0";
  	unsigned int i = 0;
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int z = 0;
	
	Select_Main_Window_16bpp();
	Main_Image_Start_Address(0);				
	Main_Image_Width(LCD_XSIZE_TFT);
	Main_Window_Start_XY(0,0);
	Canvas_Image_Start_address(0);
	Canvas_image_width(LCD_XSIZE_TFT);
  	Active_Window_XY(0,0);
	Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);
	
	while(1)
	{
		LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,BLUE2);
		
		LT768_Select_Internal_Font_Init(16,1,1,0,0);
		LT768_Print_Internal_Font_String(0,10,BLACK,BLUE2,"Embedded 8x16 ASCII Character");
		
		LT768_Select_Internal_Font_Init(24,1,1,0,0);
		LT768_Print_Internal_Font_String(0,30,BLUE,BLUE2,"Embedded 12x24 ASCII Character");
		
		LT768_Select_Internal_Font_Init(32,1,1,0,0);
		LT768_Print_Internal_Font_String(0,55,GREEN,BLUE2,"Embedded 16x32 ASCII Character");
		
		LT768_Print_Internal_Font_String(0,95,RED,BLUE2,"The Text Cursor");
		
		for(i = 0 ; i < 14 ; i++)
		{
			up_mdelay(100);
			LT768_Text_cursor_Init(1,15,1+i,15-i);
		}
		up_mdelay(100);
		LT768_Text_cursor_Init(1,15,10,2);
		
		
		c[0] = '0';
		for(i = 0 ; i < 10 ; i++)
		{
			up_mdelay(50);
			LT768_Print_Internal_Font_String(10+16*i,135,RED,BLUE2,&c[0]);
			c[0]++;
		}
		
		c[0] = 0;
		x = 0;
		y = 175;
		z = 0;
		for(i = 0 ; i < 127 ; i++)
		{
			up_mdelay(50);
			x = z * 16;
			z++;
			if(x>1024)
			{
				y = y + 40;
				x = 0;
				z = 0;
			}
			
			LT768_Print_Internal_Font_String(x,y,RED,BLUE2,&c[0]);
			c[0]++;
		}
		
		up_mdelay(1000);
		up_mdelay(1000);
		
		
		LT768_DMA_24bit_Block(1,0,200,300,600,280,1024,Picture_3_Addr);
		
		LT768_Graphic_cursor_Init(1,0xff,0x00,0,0,(uint8_t*)g_Image_pen_il);
		LT768_Graphic_cursor_Init(2,0xff,0x00,0,0,(uint8_t*)g_Image_arrow_il);
		LT768_Graphic_cursor_Init(3,0xff,0x00,0,0,(uint8_t*)g_Image_busy_im);
		LT768_Graphic_cursor_Init(4,0xff,0x00,0,0,(uint8_t*)g_Image_no_im);
		
		
		LT768_Set_Graphic_cursor_Pos(1,100,300);
		up_mdelay(500);
		LT768_Set_Graphic_cursor_Pos(2,100,300);
		up_mdelay(500);
		LT768_Set_Graphic_cursor_Pos(3,100,300);
		up_mdelay(500);
		LT768_Set_Graphic_cursor_Pos(4,100,300);
		up_mdelay(500);
		LT768_Set_Graphic_cursor_Pos(1,100,300);
		
		for(i = 100 ; i < 924 ; i++)
		{
			LT768_Set_Graphic_cursor_Pos(1,i,90);
			up_mdelay(2);
		}
		
		for(i = 200 ; i < 800 ; i++)
		{
			LT768_Set_Graphic_cursor_Pos(2,i,i-200);
			up_mdelay(2);
		}
		
		for(i = 800 ; i > 100 ; i--)
		{
			LT768_Set_Graphic_cursor_Pos(3,i,800-i);
			up_mdelay(2);
		}
		
		for(i = 924 ; i > 100 ; i--)
		{
			LT768_Set_Graphic_cursor_Pos(4,i,400);
			up_mdelay(2);
		}
		
		up_mdelay(1000);
		up_mdelay(1000);
		
		LT768_Disable_Text_Cursor();	
		LT768_Disable_Graphic_Cursor();
	}
}



#define MEMORY_ADDR_16	0x003E0D760
#define MEMORY_ADDR_24	0x003E537E0
#define MEMORY_ADDR_32	0x003EEBD00

#define FLASH_ADDR_16	  0x00829150
#define FLASH_ADDR_24	  0x0078DC20
#define FLASH_ADDR_32	  0x00679A10

#define SIZE_16_NUM		  0x00045080
#define SIZE_24_NUM     0x0009B520
#define SIZE_32_NUM     0x00114200

void Display_Outside_Font(void)
{
	Select_Main_Window_16bpp();
	Main_Image_Start_Address(0);				
	Main_Image_Width(LCD_XSIZE_TFT);
	Main_Window_Start_XY(0,0);
	Canvas_Image_Start_address(0);
	Canvas_image_width(LCD_XSIZE_TFT);
  	Active_Window_XY(0,0);
	Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);
	
	while(1)
	{
		LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,WHITE);
		LT768_Select_Outside_Font_Init(1,0,FLASH_ADDR_16,MEMORY_ADDR_16,SIZE_16_NUM,16,1,1,0,0);
		LT768_Print_Outside_Font_String(425,50,RED,WHITE,(uint8_t*)"16X16微软雅黑字体");
		up_mdelay(500);
		Font_Width_X2();
		Font_Height_X2();
		LT768_Print_Outside_Font_String(250,75,GREY-1200,WHITE,(uint8_t*)"16X16微软雅黑字体长宽各扩1倍");
		up_mdelay(500);
		Font_Width_X3();
		Font_Height_X3();
		LT768_Print_Outside_Font_String(150,120,BLUE,WHITE,(uint8_t*)"16X16微软雅黑字体长宽各扩2倍");
		up_mdelay(500);
		Font_Width_X4();
		Font_Height_X4();
		LT768_Print_Outside_Font_String(60,178,MAGENTA,WHITE,(uint8_t*)"16X16微软雅黑字体长宽各扩3倍");
		up_mdelay(500);
		
		LT768_Select_Outside_Font_Init(1,0,FLASH_ADDR_24,MEMORY_ADDR_24,SIZE_24_NUM,24,1,1,0,0);
		LT768_Print_Outside_Font_String(445,280,RED,WHITE,(uint8_t*)"24X24楷体");
		up_mdelay(500);
		Font_Width_X2();
		Font_Height_X2();
		LT768_Print_Outside_Font_String(260,315,GREEN,WHITE,(uint8_t*)"24X24楷体长宽各扩1倍");
		up_mdelay(500);
		Font_Width_X3();
		Font_Height_X3();
		LT768_Print_Outside_Font_String(150,375,CYAN,WHITE,(uint8_t*)"24X24楷体长宽各扩2倍");
		up_mdelay(500);
		Font_Width_X4();
		Font_Height_X4();
		LT768_Print_Outside_Font_String(30,455,YELLOW,WHITE,(uint8_t*)"24X24楷体长宽各扩3倍");
		up_mdelay(500);
		
		LT768_Select_Outside_Font_Init(1,0,FLASH_ADDR_32,MEMORY_ADDR_32,SIZE_32_NUM,32,1,1,0,0);
		LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,WHITE);
		LT768_Print_Outside_Font_String(430,105,RED,WHITE,(uint8_t*)"32X32隶书");
		up_mdelay(500);
		Font_Width_X2();
		Font_Height_X2();
		LT768_Print_Outside_Font_String(360,165,GREEN,WHITE,(uint8_t*)"32X32隶书");
		up_mdelay(500);
		Font_Width_X3();
		Font_Height_X3();
		LT768_Print_Outside_Font_String(285,250,BLUE,WHITE,(uint8_t*)"32X32隶书");
		up_mdelay(500);
		Font_Width_X4();
		Font_Height_X4();
		LT768_Print_Outside_Font_String(220,360,YELLOW,WHITE,(uint8_t*)"32X32隶书");
		up_mdelay(500);
	}
}

void Display_Triangle(void)
{
	unsigned long i,j,h;
	unsigned long resx1,resy1,resx2,resy2,resx3,resy3;

	Select_Main_Window_16bpp();
	Main_Image_Start_Address(0);			   
	Main_Image_Width(LCD_XSIZE_TFT);
	Canvas_Image_Start_address(0);	    
	Canvas_image_width(LCD_XSIZE_TFT);				          
	Active_Window_XY(0,0);
	Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);       
	
	Main_Window_Start_XY(0,0);				                

	while(1)
	{
		LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,WHITE);
		h=0;
		do
		{
			resx1=rand()%LCD_XSIZE_TFT;       // 点1的x轴
			resy1=rand()%(LCD_YSIZE_TFT);     // 点1的y轴
			resx2=rand()%LCD_XSIZE_TFT;       // 点2的x轴
			resy2=rand()%(LCD_YSIZE_TFT);     // 点2的y轴
			resx3=rand()%LCD_XSIZE_TFT;       // 点3的x轴
			resy3=rand()%(LCD_YSIZE_TFT);     // 点3的y轴
			i=rand()%65536;                   // 颜色
			j=rand()%3;
			if(j)	LT768_DrawTriangle(resx1,resy1,resx2,resy2,resx3,resy3,i);      // 无填充色三角形
			else	LT768_DrawTriangle_Fill(resx1,resy1,resx2,resy2,resx3,resy3,i); // 填充色三角形
			h++;
			up_mdelay(120);
		}
		while(h<20);

		h=0;
		do
		{
			resx1=rand()%LCD_XSIZE_TFT;       // 点1的x轴
			resy1=rand()%(LCD_YSIZE_TFT);     // 点1的y轴
			resx2=rand()%LCD_XSIZE_TFT;       // 点2的x轴
			resy2=rand()%(LCD_YSIZE_TFT);     // 点2的y轴
			resx3=rand()%LCD_XSIZE_TFT;       // 点3的x轴
			resy3=rand()%(LCD_YSIZE_TFT);     // 点3的y轴
			i=rand()%65536;                   // 颜色
			j=rand()%3;
			if(j)	LT768_DrawTriangle(resx1,resy1,resx2,resy2,resx3,resy3,i);      // 无填充色三角形
			else	LT768_DrawTriangle_Fill(resx1,resy1,resx2,resy2,resx3,resy3,i); // 填充色三角形
			h++;
			up_mdelay(1);
		}
		while(h<2500);

	}
}

void Tsst(void)
{
	unsigned long i,j,h;
	unsigned long resx1,resy1,resx2,resy2,resx3,resy3;
	
	Select_Main_Window_16bpp();
	Main_Image_Start_Address(0);				
	Main_Image_Width(LCD_XSIZE_TFT);
	Main_Window_Start_XY(0,0);
	Canvas_Image_Start_address(0);
	Canvas_image_width(LCD_XSIZE_TFT);
  	Active_Window_XY(0,0);
	Active_Window_WH(LCD_XSIZE_TFT,LCD_YSIZE_TFT);
	
	while(1)
	{
		
		BTE_Solid_Fill(0,LCD_XSIZE_TFT,0,0,RED,LCD_XSIZE_TFT,LCD_YSIZE_TFT);
		up_mdelay(600);
		BTE_Solid_Fill(0,LCD_XSIZE_TFT,0,0,GREEN,LCD_XSIZE_TFT,LCD_YSIZE_TFT);
		up_mdelay(600);
		BTE_Solid_Fill(0,LCD_XSIZE_TFT,0,0,BLUE,LCD_XSIZE_TFT,LCD_YSIZE_TFT);
		up_mdelay(600);

		BTE_Solid_Fill(LCD_XSIZE_TFT*LCD_YSIZE_TFT*2,LCD_XSIZE_TFT,0,0,RED,LCD_XSIZE_TFT,LCD_YSIZE_TFT);   // PIP1颜色
		BTE_Solid_Fill(LCD_XSIZE_TFT*LCD_YSIZE_TFT*4,LCD_XSIZE_TFT,0,0,GREEN,LCD_XSIZE_TFT,LCD_YSIZE_TFT); // PIP2颜色
		LT768_PIP_Init(1,1,LCD_XSIZE_TFT*LCD_YSIZE_TFT*2,0,0,LCD_XSIZE_TFT,0,100,50,50);     // 初始化PIP1           
		LT768_PIP_Init(1,2,LCD_XSIZE_TFT*LCD_YSIZE_TFT*4,0,0,LCD_XSIZE_TFT,430,85,80,80);   // 初始化PIP2
		
		for(i=0;i<300;i=i+4)
		{			
			LT768_Set_DisWindowPos(1,1,i,100);
			LT768_Set_DisWindowPos(1,2,430-i,85);
			up_mdelay(15);
		}

		for(i=0;i<300;i=i+2)
		{
			LT768_Set_DisWindowPos(1,1,300-i,100);
			LT768_Set_DisWindowPos(1,2,430-300+i,85);
			up_mdelay(10);
		}
		Disable_PIP1();	
		Disable_PIP2();
		
		
		LT768_DrawSquare_Fill(0,0,LCD_XSIZE_TFT,LCD_YSIZE_TFT,WHITE);
		h=0;
		do
		{
			resx1=rand()%LCD_XSIZE_TFT;       // 点1的x轴
			resy1=rand()%(LCD_YSIZE_TFT);     // 点1的y轴
			resx2=rand()%LCD_XSIZE_TFT;       // 点2的x轴
			resy2=rand()%(LCD_YSIZE_TFT);     // 点2的y轴
			resx3=rand()%LCD_XSIZE_TFT;       // 点3的x轴
			resy3=rand()%(LCD_YSIZE_TFT);     // 点3的y轴
			i=rand()%65536;                   // 颜色
			j=rand()%3;
			if(j)	LT768_DrawTriangle(resx1,resy1,resx2,resy2,resx3,resy3,i);      // 无填充色三角形
			else	LT768_DrawTriangle_Fill(resx1,resy1,resx2,resy2,resx3,resy3,i); // 填充色三角形
			h++;
			up_mdelay(120);
		}
		while(h<20);

		h=0;
		do
		{
			resx1=rand()%LCD_XSIZE_TFT;       // 点1的x轴
			resy1=rand()%(LCD_YSIZE_TFT);     // 点1的y轴
			resx2=rand()%LCD_XSIZE_TFT;       // 点2的x轴
			resy2=rand()%(LCD_YSIZE_TFT);     // 点2的y轴
			resx3=rand()%LCD_XSIZE_TFT;       // 点3的x轴
			resy3=rand()%(LCD_YSIZE_TFT);     // 点3的y轴
			i=rand()%65536;                   // 颜色
			j=rand()%3;
			if(j)	LT768_DrawTriangle(resx1,resy1,resx2,resy2,resx3,resy3,i);      // 无填充色三角形
			else	LT768_DrawTriangle_Fill(resx1,resy1,resx2,resy2,resx3,resy3,i); // 填充色三角形
			h++;
			up_mdelay(1);
		}
		while(h<2500);
		
		up_mdelay(100);
	}
}




