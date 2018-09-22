#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "key.h"

static RTC_DateTypeDef RTC_DateStructure;
static RTC_TimeTypeDef RTC_TimeStructure;
static RTC_InitTypeDef RTC_InitStructure;
static RTC_AlarmTypeDef  RTC_AlarmStructure;
//static __IO uint32_t AsynchPrediv=0,SynchPrediv=0;
//RTC时钟初始化
u8 MyRTC_Init(void)
{
  	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)//未设置时间
  	{  
    	if(RTC_Config())//RTC外设初始化
		{
			while(1)//初始化失败
			{
				delay_ms(500);
				LEDTog(LED4);
			}
		}
		return 1;
  	}
  	else//已设置时间
  	{   
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能外设时钟
	    PWR_BackupAccessCmd(ENABLE);//使能RTC操作
	    RTC_WaitForSynchro();//等待RTC APB寄存器同步
  	}
	return 0;
}
//RTC设置
u8 RTC_Config(void)
{
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能外设时钟
  	PWR_BackupAccessCmd(ENABLE);//使能RTC操作
  	RCC_LSEConfig(RCC_LSE_ON);//使用外部晶振
  	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);//等待外部晶振准备好
  	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//选择RTC时钟源   
  	RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
  	RTC_WaitForSynchro();//等待RTC APB寄存器同步
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC 异步除数 （<0X7F）
    RTC_InitStructure.RTC_SynchPrediv = 0xFF;//RTC 同步除数 （<0X7FFF）
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_12;//12小时制
    if(RTC_Init(&RTC_InitStructure)==ERROR)return 1;
	return 0;
}
//设置日期
u8 RTC_DateSet(u8 year,u8 month,u8 date,u8 weekday)
{
	RTC_DateStructure.RTC_WeekDay = weekday;//星期
	RTC_DateStructure.RTC_Month =  month;//月
	RTC_DateStructure.RTC_Date = date;//日
	RTC_DateStructure.RTC_Year = year;//年(0-99)
	if(RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure)==ERROR)return 1;//日期设置
	return 0;
}
//设置时钟时间
u8 RTC_TimeSet(u8 ampm,u8 hour,u8 minute,u8 second)
{
	RTC_TimeStructure.RTC_H12 = ampm;		
   	RTC_TimeStructure.RTC_Hours = hour;//时
    RTC_TimeStructure.RTC_Minutes = minute;//分
    RTC_TimeStructure.RTC_Seconds = second;//秒	
  	if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure)==ERROR)return 1;//时间设置
  	RTC_WriteBackupRegister(RTC_BKP_DR0,0x32F2);
	return 0;
}
//设置闹钟时间
void RTC_AlarmSet(u8 date,u8 ampm,u8 hour,u8 minute,u8 second)
{
  	EXTI_InitTypeDef EXTI_InitStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;

  	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
  	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = ampm;//设置上午还是下午(RTC_H12_AM/RTC_H12_PM)
  	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = hour;//时
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = minute;//分
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = second;//秒 	
  	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;//设定使用日期还是星期,这里使用日期
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = date;//日期(1-31)
  	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
  	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);//配置RTC闹钟寄存器
  	RTC_ITConfig(RTC_IT_ALRA, ENABLE);//使能闹钟A的中断
  	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);//使能闹钟A 
	RTC_ClearFlag(RTC_FLAG_ALRAF);//清除RTC闹钟标志
	EXTI_ClearITPendingBit(EXTI_Line17);//清除中断线17标志（内部连接至RTC闹钟）
  	//外部中断线配置
  	EXTI_ClearITPendingBit(EXTI_Line17);
  	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
  	//RTC闹钟中断设置
  	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}
//获取当前日期
void Get_RTCDate(u8* year,u8* month,u8* date,u8* weekday)
{
  	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	*year=RTC_DateStructure.RTC_Year;
	*month=RTC_DateStructure.RTC_Month;
	*weekday=RTC_DateStructure.RTC_WeekDay;
	*date=RTC_DateStructure.RTC_Date;
}
//获取当前时间
void Get_RTCTime(u8* ampm,u8* hour,u8* minute,u8* second)
{
  	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	*ampm=RTC_TimeStructure.RTC_H12;
	*hour=RTC_TimeStructure.RTC_Hours;
	*minute=RTC_TimeStructure.RTC_Minutes;
	*second=RTC_TimeStructure.RTC_Seconds;
}
//获取当前闹钟时间
void Get_RTCAlarm(u8* date,u8* ampm,u8* hour,u8* minute,u8* second)
{
  	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
  	*ampm=RTC_AlarmStructure.RTC_AlarmTime.RTC_H12;
  	*hour=RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours;
    *minute=RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes;
    *second=RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds; 	
	*date=RTC_AlarmStructure.RTC_AlarmDateWeekDay;
}



