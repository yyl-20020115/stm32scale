#include "key.h"
#include "delay.h"
//PIN:PB0,PB1,PB2,PB10
//按键GPIO初始化
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//初始化结构体

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//GPIO外设时钟使能
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_10;//对应GPIO引脚  
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//状态为输入
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
  	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
}
//将按键设置成中断方式
void Set_Keyint(Key_Def key)
{
  	EXTI_InitTypeDef EXTI_InitStructure;//外部中断初始化结构体
  	NVIC_InitTypeDef NVIC_InitStructure;//中断向量初始化结构体

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//外设时钟使能
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, KEY_PNS[key]);
    EXTI_InitStructure.EXTI_Line = KEY_SRC[key];//中断线
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式    
	if(key==KEY3)EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿 
	else EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
    EXTI_Init(&EXTI_InitStructure);//中断初始化
    NVIC_InitStructure.NVIC_IRQChannel = KEY_IRQ[key];//中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//抢占优先级最低(0~15)
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;//子优先级最低(0~15)
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//中断通道使能
    NVIC_Init(&NVIC_InitStructure);//中断向量初始化 
}
//获取KEYn的输入状态,其中KEY3为下拉,其它为上拉
u8 Get_Key(Key_Def key)
{
  	if ((GPIOB->IDR&KEY_PIN[key])!=0)return 1;
  	else return 0;
}
//按键扫描,返回按键编号,无按键按下则返回0
u8 KEY_Scan(void)
{	 
	static u8 key_up=1;//全局保存按键状态
	if(key_up&&(!Get_Key(KEY1)||!Get_Key(KEY2)||Get_Key(KEY3)||!Get_Key(KEY4)))//有新按键按下
	{
		Delay(0xFFFFF);//延时消抖 
		key_up=0;
		if(Get_Key(KEY1)==0)return 1;
		else if(Get_Key(KEY2)==0)return 2;
		else if(Get_Key(KEY3)==1)return 3;
		else if(Get_Key(KEY4)==0)return 4;
	}
	else if(Get_Key(KEY1)&&Get_Key(KEY2)&&!Get_Key(KEY3)&&Get_Key(KEY4))key_up=1;//无按键按下 	    
	return 0;
}


