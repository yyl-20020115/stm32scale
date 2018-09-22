#include "usb_bsp.h"
#include "main.h"

#define TIM_MSEC_DELAY                     0x01
#define TIM_USEC_DELAY                     0x02

ErrorStatus HSEStartUpStatus;
__IO uint32_t BSP_delay=0;

static void BSP_SetTime(uint8_t Unit);
static void BSP_Delay(uint32_t nTime, uint8_t Unit);
static void USB_OTG_BSP_TimeInit ( void );

void BSP_Init(void)
{
}

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)//STM32F407仅支持USB OTG FS core
{
 	//初始化ID,VBUS,DP,DM
  	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);  
  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12; 
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
//  	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_OTG1_FS) ; 
  	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG1_FS) ; 
  	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG1_FS) ;

//  	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
//  	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;  
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  	GPIO_Init(GPIOA, &GPIO_InitStructure);  
//  	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_OTG1_FS) ;   
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE) ; 
  	USB_OTG_BSP_TimeInit(); 
}

void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
  	NVIC_InitTypeDef NVIC_InitStructure;

  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  	NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{
}

void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{
  	USB_OTG_BSP_mDelay(200);   
}

static void USB_OTG_BSP_TimeInit (void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;

  	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void USB_OTG_BSP_uDelay (const uint32_t usec)
{
  	BSP_Delay(usec, TIM_USEC_DELAY);
//  	u32 count=0;
//  	const u32 utime=(120*usec/7);
//    while(1)if(++count>utime)break;
}

void USB_OTG_BSP_mDelay (const uint32_t msec)
{
  	BSP_Delay(msec, TIM_MSEC_DELAY);
//	USB_OTG_BSP_uDelay(msec * 1000);
}

void USB_OTG_BSP_TimerIRQ (void)
{
  	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  	{
    	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    	if (BSP_delay > 0x00)BSP_delay--;
    	else TIM_Cmd(TIM2, DISABLE);
  	}
}

static void BSP_Delay(uint32_t nTime, uint8_t unit)
{
  	BSP_delay=nTime;
  	BSP_SetTime(unit);
  	while (BSP_delay!=0);
  	TIM_Cmd(TIM2,DISABLE);
}

static void BSP_SetTime(uint8_t unit)
{
  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  	TIM_Cmd(TIM2, DISABLE);
  	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
  	if (unit == TIM_USEC_DELAY)TIM_TimeBaseStructure.TIM_Period = 11;
  	else if (unit == TIM_MSEC_DELAY)TIM_TimeBaseStructure.TIM_Period = 11999;
  	TIM_TimeBaseStructure.TIM_Prescaler = 5;
  	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  	TIM_ARRPreloadConfig(TIM2, ENABLE);
  	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  	TIM_Cmd(TIM2, ENABLE);
}


