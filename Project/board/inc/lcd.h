#ifndef __LCD_H
#define __LCD_H

#include "stm32f4xx_conf.h"
#include "fonts.h"
 
#define WHITE         	 	0xFFFF
#define BLACK         	 	0x0000	  
#define BLUE         	 	0x001F  
#define BRED             	0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED           	 	0xF800
#define MAGENTA       	 	0xF81F
#define GREEN         	 	0x07E0
#define CYAN          	 	0x7FFF
#define YELLOW        	 	0xFFE0
#define BROWN 			 	0XBC40 	//棕色
#define BRRED 			 	0XFC07 	//棕红色
#define GRAY  			 	0X8430 	//灰色
#define DARKBLUE      	 	0X01CF 	//深蓝色
#define LIGHTBLUE      	 	0X7D7C 	//浅蓝色  
#define GRAYBLUE       	 	0X5458 	//灰蓝色

#define	Fense				0xe9ce	//粉色
#define Chase			  	0xd5b1	//茶色
#define Hese		     	0xa285	//褐色
#define Tianlanse		 	0x05ff	//天蓝色
#define Jialanse		  	0x435c	//皇家蓝
#define Bohese			   	0xf5ff	//薄荷色
#define Huise			   	0x0000	//灰色
#define Yinbaise			0xc618	//银白色 
#define Qianhuise		 	0x8410	//浅灰色
#define Chengse			  	0xfbea	//橙色


#define LCD_RAM   *(vu16*)((u32)0x60020000)  //disp Data ADDR
#define LCD_REG   *(vu16*)((u32)0x60000000)	 //disp Reg  ADDR

#define LCD_WIDTH	  320
#define LCD_HEIGHT	  240


#define RGB565toRGB555(RGB) ((RGB&0xF800)>>1)+((RGB&0x7C0)>>1)+(RGB&0x1f)
#define BGR565toRGB555(BGR) ((BGR&0x1f)<<10)+((BGR&0x7C0)>>1)+((BGR&0xF800)>>11)
#define BGR565toRGB565(BGR) ((BGR&0x1f)<<11)+(BGR&0x7e0)+((BGR&0xF800)>>11)

extern u8 	Fsize;
extern u8 	Charmd;  
extern u16 	Bcolor; 


void LCD_Init(void);
void LCD_Font(u8 font);
void LCD_Dispmd(u8 mode);
void LCD_Charmd(u8 mode,u16 color);
void LCD_Cursor(u16 Xpos, u16 Ypos);
void LCD_Window(u16 Xpos, u16 Ypos, u16 Height, u16 Width);

void LCD_Clear(u16 Color);
void LCD_SetPoint(u16 xpos, u16 ypos,u16 color);
u16  LCD_GetPoint(u16 x,u16 y);
void LCD_Line(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_Rect(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_Circle(u16 Xpos, u16 Ypos, u16 Radius, u16 color);
void LCD_Fill(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);

void LCD_Char(u16 x,u16 y,u8 num,u16 color);
void LCD_String(u16 x,u16 y,const u8 *p,u16 color);
void LCD_Num(u16 x, u16 y, u32 num, u8 len, u16 color);
void Draw_Bmp(u16 x,u16 y,u16 width,u16 height,u8* pic);

void LCD_LineConf(void);
void LCD_FSMCConf(void);
void LCD_WriteReg(u8 Reg, u16 Value);
u16  LCD_ReadReg(u8 Reg);
void LCD_WriteRAM(u16 RGB_Code);
u16  LCD_ReadRAM(void);


#endif
