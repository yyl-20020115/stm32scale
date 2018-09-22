#include "key.h"
#include "delay.h"
//PIN:PB0,PB1,PB2,PB10
//����GPIO��ʼ��
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//��ʼ���ṹ��

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//GPIO����ʱ��ʹ��
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_10;//��ӦGPIO����  
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//״̬Ϊ����
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
  	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
}
//���������ó��жϷ�ʽ
void Set_Keyint(Key_Def key)
{
  	EXTI_InitTypeDef EXTI_InitStructure;//�ⲿ�жϳ�ʼ���ṹ��
  	NVIC_InitTypeDef NVIC_InitStructure;//�ж�������ʼ���ṹ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//����ʱ��ʹ��
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, KEY_PNS[key]);
    EXTI_InitStructure.EXTI_Line = KEY_SRC[key];//�ж���
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж�ģʽ    
	if(key==KEY3)EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//������ 
	else EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
    EXTI_Init(&EXTI_InitStructure);//�жϳ�ʼ��
    NVIC_InitStructure.NVIC_IRQChannel = KEY_IRQ[key];//�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//��ռ���ȼ����(0~15)
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;//�����ȼ����(0~15)
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//�ж�ͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);//�ж�������ʼ�� 
}
//��ȡKEYn������״̬,����KEY3Ϊ����,����Ϊ����
u8 Get_Key(Key_Def key)
{
  	if ((GPIOB->IDR&KEY_PIN[key])!=0)return 1;
  	else return 0;
}
//����ɨ��,���ذ������,�ް��������򷵻�0
u8 KEY_Scan(void)
{	 
	static u8 key_up=1;//ȫ�ֱ��水��״̬
	if(key_up&&(!Get_Key(KEY1)||!Get_Key(KEY2)||Get_Key(KEY3)||!Get_Key(KEY4)))//���°�������
	{
		Delay(0xFFFFF);//��ʱ���� 
		key_up=0;
		if(Get_Key(KEY1)==0)return 1;
		else if(Get_Key(KEY2)==0)return 2;
		else if(Get_Key(KEY3)==1)return 3;
		else if(Get_Key(KEY4)==0)return 4;
	}
	else if(Get_Key(KEY1)&&Get_Key(KEY2)&&!Get_Key(KEY3)&&Get_Key(KEY4))key_up=1;//�ް������� 	    
	return 0;
}


