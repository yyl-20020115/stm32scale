#include "led.h"
//PIN:PA3,PC2,PC3,PC13
//LED初始化
void LED_Init(void)
{
  	GPIO_InitTypeDef  GPIO_InitStructure;//GPIO初始化结构体

  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//外设时钟使能
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//外设时钟使能
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//对应GPIO引脚
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//状态为输出
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//挽输出
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//IO速度为100MHZ
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
  	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_13;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void LEDOn(Led_Def Led)//LED亮
{
  	LED_PORT[Led]->BSRRL=LED_PIN[Led];
}

void LEDOff(Led_Def Led)//LED灭
{
  	LED_PORT[Led]->BSRRH=LED_PIN[Led];  
}

void LEDTog(Led_Def Led)//LED状态翻转
{
  	LED_PORT[Led]->ODR^=LED_PIN[Led];
}



