#include <stdlib.h>
#include "touch.h" 
#include "lcd.h"
#include "delay.h"	 
#include "spi.h"

//定义用来存放XY轴坐标的变量
u16 PENX,PENY;
//读AD转换器的值(12位)	 	   
u16 ADS_Read(u8 CMD)	  
{ 	 	  
	u16 Num=0,temp;  	 
	TCS_LOW;//先中 
	SPI1_RWByte(CMD);//发送转换命令
	delay_us(6);//等待转换完成	 
	temp=SPI1_RWByte(0x00);//读数
	Num=temp<<8; 
	delay_us(1); 
	temp=SPI1_RWByte(0x00); 
	Num|=temp;  	
	Num>>=4;//高12位有效
	TCS_HIGH; 
	return(Num);  
} 
//读取X轴或Y轴转换值,带滤波
u16 Read_XY(u8 CMD)
{
	u16 i, j;
	u16 buf[10];
	u16 sum=0;
	u16 temp;
	for(i=0;i<10;i++)buf[i]=ADS_Read(CMD);//读取10次				    
	for(i=0;i<9; i++)//升序排序
	{
		for(j=i+1;j<10;j++)
		{
			if(buf[i]>buf[j])
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	for(i=3;i<7;i++)sum+=buf[i];//舍弃首尾并取平均值
	temp=sum/(4);
	return temp;   
} 
//触摸屏初始化
void Touch_Init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
//  	EXTI_InitTypeDef EXTI_InitStructure;
//  	NVIC_InitTypeDef NVIC_InitStructure;
	//初始化PEN_INT触发引脚
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;   
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	//将其配置成中断
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);
//    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//优先级最低
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0E;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure); 	
			   
	SPI1_Init();//初始化SPI1
	ADS_SPI_Init();//将SPI1初始化为触摸屏用途
	CSPin_init();//初始化所有SPI器件片选引脚并置高不选中
}

//连续读取2次有效的AD值,且这两次的偏差不能超过ERR_RANGE
#define ERR_RANGE 50 //误差范围 
#define MIN_VALUE 50 //AD最小值
u8 Read_X(u16* xpos)
{	
	int temp=0,v1,v2;

	while(1)
	{
		if(PEN_int!=RESET)return 1;//返回错误
	    v1=Read_XY(CMD_RDX);   
	    if(v1<MIN_VALUE)continue;//AD值不能小于MIN_VALUE
	    v2=Read_XY(CMD_RDX);   
	    if(v2<MIN_VALUE)continue;
		if(abs(v1-v2)>ERR_RANGE)continue;//相邻采样点相差不能超过ERR_RANGE
		temp=(v1+v2)/2;//取平均值
		break;
	}
	*xpos=240*(temp-ADx1)/(ADx2-ADx1);//按分辨率进行换算
	return 0; 
}

u8 Read_Y(u16* ypos)
{
	int temp=0,v1,v2;

	while(1)
	{
		if(PEN_int!=RESET)return 1;//返回错误
	    v1=Read_XY(CMD_RDY);   
	    if(v1<MIN_VALUE)continue;//AD值不能小于MIN_VALUE
	    v2=Read_XY(CMD_RDY);   
	    if(v2<MIN_VALUE)continue;
		if(abs(v1-v2)>ERR_RANGE)continue;//相邻采样点相差不能超过ERR_RANGE
		temp=(v1+v2)/2;//取平均值	
		break;		
	}
	*ypos=320*(temp-ADy1)/(ADy2-ADy1);//按分辨率进行换算
	return 0; 	
}
 


