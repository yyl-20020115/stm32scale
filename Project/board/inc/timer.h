#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f4xx_conf.h"

void TIM3_Init(u32 TIM_scale, u32 TIM_Period);
void TIM3_Cinit(u32 TIM_scale ,u16 CCR1 ,u16 CCR2);
void TIM5_Config(void);
void PWM_Init(void);
void PWM_SetPulse(u16 Pulse);

void PWM2_Init(void);
void PWM2_SetPulse(u16 Pulse);

#endif					 

