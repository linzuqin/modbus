#ifndef _TIM_H_
#define _TIM_H_
#include "stm32f10x.h"                  // Device header
#include "rtthread.h"
#include "log.h"
#include "GPIO.h"

void TIMx_Init(TIM_TypeDef *TIMx, uint16_t period, uint16_t prescaler, uint16_t pulse, uint16_t channel);






#endif
