#include "led.h"
//PIN:PA3,PC2,PC3,PC13
//LED��ʼ��
void LED_Init(void)
{
  	GPIO_InitTypeDef  GPIO_InitStructure;//GPIO��ʼ���ṹ��

  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//����ʱ��ʹ��
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//����ʱ��ʹ��
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//��ӦGPIO����
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//״̬Ϊ���
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�����
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//IO�ٶ�Ϊ100MHZ
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_13;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void LEDOn(Led_Def Led)//LED��
{
  	LED_PORT[Led]->BSRRL=LED_PIN[Led];
}

void LEDOff(Led_Def Led)//LED��
{
  	LED_PORT[Led]->BSRRH=LED_PIN[Led];  
}

void LEDTog(Led_Def Led)//LED״̬��ת
{
  	LED_PORT[Led]->ODR^=LED_PIN[Led];
}



