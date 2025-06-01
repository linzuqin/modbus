#include "TIM.h"
extern void prvvTIMERExpiredISR(void);

void TIMx_Init(TIM_TypeDef *TIMx, uint16_t period, uint16_t prescaler, uint16_t pulse, uint16_t channel)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	if (TIMx == TIM1 || TIMx == TIM8)
	{
		RCC_APB2PeriphClockCmd((TIMx == TIM1) ? RCC_APB2Periph_TIM1 : RCC_APB2Periph_TIM8, ENABLE);
	}
	else
	{
		if (TIMx == TIM2)
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		else if (TIMx == TIM3)
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		else if (TIMx == TIM4)
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	}

	TIM_InternalClockConfig(TIMx);
	TIM_DeInit(TIM2);
	TIM_TimeBaseStructure.TIM_Period = period;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;

	if (TIMx == TIM1)
		NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	else if (TIMx == TIM2)
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	else if (TIMx == TIM3)
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	else if (TIMx == TIM4)
		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIMx, ENABLE);
}

void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		prvvTIMERExpiredISR();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}