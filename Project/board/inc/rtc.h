#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_conf.h"


u8   MyRTC_Init(void);
u8   RTC_Config(void);
u8   RTC_DateSet(u8 year,u8 month,u8 date,u8 weekday);
u8   RTC_TimeSet(u8 ampm,u8 hour,u8 minute,u8 second);
void RTC_AlarmSet(u8 date,u8 ampm,u8 hour,u8 minute,u8 second);
void Get_RTCDate(u8* year,u8* month,u8* date,u8* weekday);
void Get_RTCTime(u8* ampm,u8* hour,u8* minute,u8* second);
void Get_RTCAlarm(u8* date,u8* ampm,u8* hour,u8* minute,u8* second);


#endif

