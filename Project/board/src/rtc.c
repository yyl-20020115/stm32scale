#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "key.h"

static RTC_DateTypeDef RTC_DateStructure;
static RTC_TimeTypeDef RTC_TimeStructure;
static RTC_InitTypeDef RTC_InitStructure;
static RTC_AlarmTypeDef  RTC_AlarmStructure;
//static __IO uint32_t AsynchPrediv=0,SynchPrediv=0;
//RTCʱ�ӳ�ʼ��
u8 MyRTC_Init(void)
{
  	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)//δ����ʱ��
  	{  
    	if(RTC_Config())//RTC�����ʼ��
		{
			while(1)//��ʼ��ʧ��
			{
				delay_ms(500);
				LEDTog(LED4);
			}
		}
		return 1;
  	}
  	else//������ʱ��
  	{   
	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ������ʱ��
	    PWR_BackupAccessCmd(ENABLE);//ʹ��RTC����
	    RTC_WaitForSynchro();//�ȴ�RTC APB�Ĵ���ͬ��
  	}
	return 0;
}
//RTC����
u8 RTC_Config(void)
{
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ������ʱ��
  	PWR_BackupAccessCmd(ENABLE);//ʹ��RTC����
  	RCC_LSEConfig(RCC_LSE_ON);//ʹ���ⲿ����
  	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);//�ȴ��ⲿ����׼����
  	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//ѡ��RTCʱ��Դ   
  	RCC_RTCCLKCmd(ENABLE);//ʹ��RTCʱ��
  	RTC_WaitForSynchro();//�ȴ�RTC APB�Ĵ���ͬ��
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC �첽���� ��<0X7F��
    RTC_InitStructure.RTC_SynchPrediv = 0xFF;//RTC ͬ������ ��<0X7FFF��
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_12;//12Сʱ��
    if(RTC_Init(&RTC_InitStructure)==ERROR)return 1;
	return 0;
}
//��������
u8 RTC_DateSet(u8 year,u8 month,u8 date,u8 weekday)
{
	RTC_DateStructure.RTC_WeekDay = weekday;//����
	RTC_DateStructure.RTC_Month =  month;//��
	RTC_DateStructure.RTC_Date = date;//��
	RTC_DateStructure.RTC_Year = year;//��(0-99)
	if(RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure)==ERROR)return 1;//��������
	return 0;
}
//����ʱ��ʱ��
u8 RTC_TimeSet(u8 ampm,u8 hour,u8 minute,u8 second)
{
	RTC_TimeStructure.RTC_H12 = ampm;		
   	RTC_TimeStructure.RTC_Hours = hour;//ʱ
    RTC_TimeStructure.RTC_Minutes = minute;//��
    RTC_TimeStructure.RTC_Seconds = second;//��	
  	if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure)==ERROR)return 1;//ʱ������
  	RTC_WriteBackupRegister(RTC_BKP_DR0,0x32F2);
	return 0;
}
//��������ʱ��
void RTC_AlarmSet(u8 date,u8 ampm,u8 hour,u8 minute,u8 second)
{
  	EXTI_InitTypeDef EXTI_InitStructure;
  	NVIC_InitTypeDef NVIC_InitStructure;

  	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
  	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = ampm;//�������绹������(RTC_H12_AM/RTC_H12_PM)
  	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = hour;//ʱ
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = minute;//��
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = second;//�� 	
  	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;//�趨ʹ�����ڻ�������,����ʹ������
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = date;//����(1-31)
  	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
  	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);//����RTC���ӼĴ���
  	RTC_ITConfig(RTC_IT_ALRA, ENABLE);//ʹ������A���ж�
  	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);//ʹ������A 
	RTC_ClearFlag(RTC_FLAG_ALRAF);//���RTC���ӱ�־
	EXTI_ClearITPendingBit(EXTI_Line17);//����ж���17��־���ڲ�������RTC���ӣ�
  	//�ⲿ�ж�������
  	EXTI_ClearITPendingBit(EXTI_Line17);
  	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
  	//RTC�����ж�����
  	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}
//��ȡ��ǰ����
void Get_RTCDate(u8* year,u8* month,u8* date,u8* weekday)
{
  	RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
	*year=RTC_DateStructure.RTC_Year;
	*month=RTC_DateStructure.RTC_Month;
	*weekday=RTC_DateStructure.RTC_WeekDay;
	*date=RTC_DateStructure.RTC_Date;
}
//��ȡ��ǰʱ��
void Get_RTCTime(u8* ampm,u8* hour,u8* minute,u8* second)
{
  	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	*ampm=RTC_TimeStructure.RTC_H12;
	*hour=RTC_TimeStructure.RTC_Hours;
	*minute=RTC_TimeStructure.RTC_Minutes;
	*second=RTC_TimeStructure.RTC_Seconds;
}
//��ȡ��ǰ����ʱ��
void Get_RTCAlarm(u8* date,u8* ampm,u8* hour,u8* minute,u8* second)
{
  	RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
  	*ampm=RTC_AlarmStructure.RTC_AlarmTime.RTC_H12;
  	*hour=RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours;
    *minute=RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes;
    *second=RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds; 	
	*date=RTC_AlarmStructure.RTC_AlarmDateWeekDay;
}



