#include "delay.h"

__IO u32 ntime;								    
//ʹ��SysTick���о�ȷ��ʱ
//ms��ʱ
void delay_ms(u32 nms)
{	 		  	  
	ntime=nms;
	SysTick_Config(168000);//1ms����һ���ж�,����ntime��1
	while(ntime);//�ȴ�ʱ�䵽��
	SysTick->CTRL=0x00;			  	    
}   
//us��ʱ		    								   
void delay_us(u32 nus)
{		
	ntime=nus;
	SysTick_Config(168);//1us����һ���ж�,����ntime��1
	while(ntime);
	SysTick->CTRL=0x00;
}
//������ʱ
void Delay(u32 count)
{
	while(count--);
}

