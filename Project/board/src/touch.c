#include <stdlib.h>
#include "touch.h" 
#include "lcd.h"
#include "delay.h"	 
#include "spi.h"

//�����������XY������ı���
u16 PENX,PENY;
//��ADת������ֵ(12λ)	 	   
u16 ADS_Read(u8 CMD)	  
{ 	 	  
	u16 Num=0,temp;  	 
	TCS_LOW;//���� 
	SPI1_RWByte(CMD);//����ת������
	delay_us(6);//�ȴ�ת�����	 
	temp=SPI1_RWByte(0x00);//����
	Num=temp<<8; 
	delay_us(1); 
	temp=SPI1_RWByte(0x00); 
	Num|=temp;  	
	Num>>=4;//��12λ��Ч
	TCS_HIGH; 
	return(Num);  
} 
//��ȡX���Y��ת��ֵ,���˲�
u16 Read_XY(u8 CMD)
{
	u16 i, j;
	u16 buf[10];
	u16 sum=0;
	u16 temp;
	for(i=0;i<10;i++)buf[i]=ADS_Read(CMD);//��ȡ10��				    
	for(i=0;i<9; i++)//��������
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
	for(i=3;i<7;i++)sum+=buf[i];//������β��ȡƽ��ֵ
	temp=sum/(4);
	return temp;   
} 
//��������ʼ��
void Touch_Init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
//  	EXTI_InitTypeDef EXTI_InitStructure;
//  	NVIC_InitTypeDef NVIC_InitStructure;
	//��ʼ��PEN_INT��������
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;   
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	//�������ó��ж�
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);
//    EXTI_InitStructure.EXTI_Line = EXTI_Line6;
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//���ȼ����
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0E;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure); 	
			   
	SPI1_Init();//��ʼ��SPI1
	ADS_SPI_Init();//��SPI1��ʼ��Ϊ��������;
	CSPin_init();//��ʼ������SPI����Ƭѡ���Ų��ø߲�ѡ��
}

//������ȡ2����Ч��ADֵ,�������ε�ƫ��ܳ���ERR_RANGE
#define ERR_RANGE 50 //��Χ 
#define MIN_VALUE 50 //AD��Сֵ
u8 Read_X(u16* xpos)
{	
	int temp=0,v1,v2;

	while(1)
	{
		if(PEN_int!=RESET)return 1;//���ش���
	    v1=Read_XY(CMD_RDX);   
	    if(v1<MIN_VALUE)continue;//ADֵ����С��MIN_VALUE
	    v2=Read_XY(CMD_RDX);   
	    if(v2<MIN_VALUE)continue;
		if(abs(v1-v2)>ERR_RANGE)continue;//���ڲ��������ܳ���ERR_RANGE
		temp=(v1+v2)/2;//ȡƽ��ֵ
		break;
	}
	*xpos=240*(temp-ADx1)/(ADx2-ADx1);//���ֱ��ʽ��л���
	return 0; 
}

u8 Read_Y(u16* ypos)
{
	int temp=0,v1,v2;

	while(1)
	{
		if(PEN_int!=RESET)return 1;//���ش���
	    v1=Read_XY(CMD_RDY);   
	    if(v1<MIN_VALUE)continue;//ADֵ����С��MIN_VALUE
	    v2=Read_XY(CMD_RDY);   
	    if(v2<MIN_VALUE)continue;
		if(abs(v1-v2)>ERR_RANGE)continue;//���ڲ��������ܳ���ERR_RANGE
		temp=(v1+v2)/2;//ȡƽ��ֵ	
		break;		
	}
	*ypos=320*(temp-ADy1)/(ADy2-ADy1);//���ֱ��ʽ��л���
	return 0; 	
}
 


