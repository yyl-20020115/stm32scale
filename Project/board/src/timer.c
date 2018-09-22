#include "timer.h"

//通用定时器3初始化,对84MHz（TIM3时钟源为168MHz/2）进行TIM_scale分频后作为计数时钟
void TIM3_Init(u32 TIM_scale, u32 TIM_Period)//TIM_Period为16位的数
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	NVIC_InitTypeDef  NVIC_InitStructure; 
	 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//外设时钟使能  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级最高
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//子优先级
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);					
  	TIM_TimeBaseStructure.TIM_Period = TIM_Period;//计数器重装值
  	TIM_TimeBaseStructure.TIM_Prescaler = 0;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//递增计数方式
  	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  	TIM_PrescalerConfig(TIM3, (TIM_scale-1), TIM_PSCReloadMode_Immediate);//设置16位时钟分频系数,立即重载模式
  	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//计满中断
  	TIM_Cmd(TIM3, ENABLE);
}

//初始化定时器比较模式,CCR为16位的数
void TIM3_Cinit(u32 TIM_scale ,u16 CCR1 ,u16 CCR2)
{	 
  	NVIC_InitTypeDef  NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	  
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//外设时钟使能  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//设置中断优先级
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);					
  	TIM_TimeBaseStructure.TIM_Period = 65535;//设置计数值为最大
  	TIM_TimeBaseStructure.TIM_Prescaler = 0;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  	TIM_PrescalerConfig(TIM3, (TIM_scale-1), TIM_PSCReloadMode_Immediate);
  	//通道1
  	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  	TIM_OCInitStructure.TIM_Pulse = CCR1;//设置脉宽(16位)
  	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//高优先级
  	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
	//通道2 	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  	TIM_OCInitStructure.TIM_Pulse = CCR2;
  	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
//	//通道3
//  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  	TIM_OCInitStructure.TIM_Pulse = CCR3;
//  	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
//  	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
	//中断使能
  	TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);		
}

//配置定时器5输出通道
void TIM5_Config(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//使能定时器5
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//GPIOA时钟使能
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//配置PA3为定时器5第4通道输出引脚
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用模式
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//上拉
  	GPIO_Init(GPIOA, &GPIO_InitStructure); 
  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM5);//连接复用引脚
}
//通过定时器5输出PWM
static TIM_OCInitTypeDef  TIM_OCInitStructure;
void PWM_Init(void)
{
	uint16_t PrescalerValue=0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	

	TIM5_Config();  
	//计算预分频因子,设置定时器5计数时钟为28MHZ
	//Prescaler = (TIM5CLK / TIM5 counter clock) - 1	
  	PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 28000000) - 1;
  	//配置时基
  	TIM_TimeBaseStructure.TIM_Period = 665;
  	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	//PWM1模式配置
  	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  	TIM_OCInitStructure.TIM_Pulse = 0;
  	TIM_OC4Init(TIM5, &TIM_OCInitStructure);
  	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
  	TIM_ARRPreloadConfig(TIM5, ENABLE);
  	TIM_Cmd(TIM5, ENABLE);//计数使能
}
//设置PWM脉宽
void PWM_SetPulse(u16 Pulse)
{
	TIM_Cmd(TIM5, DISABLE);//计数禁止
	TIM_OCInitStructure.TIM_Pulse = Pulse;
  	TIM_OC4Init(TIM5, &TIM_OCInitStructure);
  	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
  	TIM_ARRPreloadConfig(TIM5, ENABLE);
  	TIM_Cmd(TIM5, ENABLE);//计数使能
}


