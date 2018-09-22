#include "timer.h"

//ͨ�ö�ʱ��3��ʼ��,��84MHz��TIM3ʱ��ԴΪ168MHz/2������TIM_scale��Ƶ����Ϊ����ʱ��
void TIM3_Init(u32 TIM_scale, u32 TIM_Period)//TIM_PeriodΪ16λ����
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	NVIC_InitTypeDef  NVIC_InitStructure; 
	 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//����ʱ��ʹ��  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ����
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//�����ȼ�
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);					
  	TIM_TimeBaseStructure.TIM_Period = TIM_Period;//��������װֵ
  	TIM_TimeBaseStructure.TIM_Prescaler = 0;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//����������ʽ
  	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  	TIM_PrescalerConfig(TIM3, (TIM_scale-1), TIM_PSCReloadMode_Immediate);//����16λʱ�ӷ�Ƶϵ��,��������ģʽ
  	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//�����ж�
  	TIM_Cmd(TIM3, ENABLE);
}

//��ʼ����ʱ���Ƚ�ģʽ,CCRΪ16λ����
void TIM3_Cinit(u32 TIM_scale ,u16 CCR1 ,u16 CCR2)
{	 
  	NVIC_InitTypeDef  NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	  
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//����ʱ��ʹ��  
  	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//�����ж����ȼ�
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);					
  	TIM_TimeBaseStructure.TIM_Period = 65535;//���ü���ֵΪ���
  	TIM_TimeBaseStructure.TIM_Prescaler = 0;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  	TIM_PrescalerConfig(TIM3, (TIM_scale-1), TIM_PSCReloadMode_Immediate);
  	//ͨ��1
  	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  	TIM_OCInitStructure.TIM_Pulse = CCR1;//��������(16λ)
  	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//�����ȼ�
  	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
	//ͨ��2 	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  	TIM_OCInitStructure.TIM_Pulse = CCR2;
  	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);
//	//ͨ��3
//  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  	TIM_OCInitStructure.TIM_Pulse = CCR3;
//  	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
//  	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
	//�ж�ʹ��
  	TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);		
}

//���ö�ʱ��5���ͨ��
void TIM5_Config(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//ʹ�ܶ�ʱ��5
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//GPIOAʱ��ʹ��
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//����PA3Ϊ��ʱ��5��4ͨ���������
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//����ģʽ
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//����
  	GPIO_Init(GPIOA, &GPIO_InitStructure); 
  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM5);//���Ӹ�������
}
//ͨ����ʱ��5���PWM
static TIM_OCInitTypeDef  TIM_OCInitStructure;
void PWM_Init(void)
{
	uint16_t PrescalerValue=0;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	

	TIM5_Config();  
	//����Ԥ��Ƶ����,���ö�ʱ��5����ʱ��Ϊ28MHZ
	//Prescaler = (TIM5CLK / TIM5 counter clock) - 1	
  	PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 28000000) - 1;
  	//����ʱ��
  	TIM_TimeBaseStructure.TIM_Period = 665;
  	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	//PWM1ģʽ����
  	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  	TIM_OCInitStructure.TIM_Pulse = 0;
  	TIM_OC4Init(TIM5, &TIM_OCInitStructure);
  	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
  	TIM_ARRPreloadConfig(TIM5, ENABLE);
  	TIM_Cmd(TIM5, ENABLE);//����ʹ��
}
//����PWM����
void PWM_SetPulse(u16 Pulse)
{
	TIM_Cmd(TIM5, DISABLE);//������ֹ
	TIM_OCInitStructure.TIM_Pulse = Pulse;
  	TIM_OC4Init(TIM5, &TIM_OCInitStructure);
  	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
  	TIM_ARRPreloadConfig(TIM5, ENABLE);
  	TIM_Cmd(TIM5, ENABLE);//����ʹ��
}


