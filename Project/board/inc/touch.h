#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "stm32f4xx_conf.h"
//PIN:PB14,PD6
#define TCS_LOW    	GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define TCS_HIGH   	GPIO_SetBits(GPIOB, GPIO_Pin_14)     
#define PEN_int		(GPIOD->IDR&GPIO_Pin_6)

#define CMD_RDY 0X90  
#define CMD_RDX	0XD0    											 	    

#define ADx1	128
#define ADx2	1930
#define ADy1	90
#define ADy2	1880

extern u16 PENX,PENY;//用于存储当前触屏座标
			  
void Touch_Init(void);		 		 
u16 ADS_Read(u8 CMD);	 
u16 Read_XY(u8 CMD);
u8 Read_X(u16* xpos);
u8 Read_Y(u16* ypos);
	 
#endif


