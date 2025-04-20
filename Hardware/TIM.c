#include "TIM.h"

extern void prvvTIMERExpiredISR(void);

void TIM2_PWM_Init(uint16_t pulse, uint8_t channel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	if (channel > 0 && channel < 5)
	{
		MyGPIO_Init(GPIOA, (GPIO_Pin_0 << (channel - 1)), GPIO_Mode_AF_PP);
	}
	else
	{
		LOG_I("error: invalid PWM port for TIM2");
		return;
	}

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	if (channel == 1)
	{
		TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	}
	else if (channel == 2)
	{
		TIM_OC2Init(TIM2, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	}
	else if (channel == 3)
	{
		TIM_OC3Init(TIM2, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	}
	else if (channel == 4)
	{
		TIM_OC4Init(TIM2, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
	}

	TIM_Cmd(TIM2, ENABLE);
}

void TIM3_PWM_Init(uint16_t pulse, uint8_t channel)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	if (channel == 1 || channel == 2)
	{
		MyGPIO_Init(GPIOB, (GPIO_Pin_4 << (channel - 1)), GPIO_Mode_AF_PP);
	}
	else if (channel == 3 || channel == 4)
	{
		MyGPIO_Init(GPIOB, (GPIO_Pin_0 << (channel - 3)), GPIO_Mode_AF_PP);
	}
	else
	{
		LOG_I("error: invalid PWM port for TIM3");
		return;
	}

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	if (channel == 1)
	{
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}
	else if (channel == 2)
	{
		TIM_OC2Init(TIM3, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}
	else if (channel == 3)
	{
		TIM_OC3Init(TIM3, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}
	else if (channel == 4)
	{
		TIM_OC4Init(TIM3, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}

	TIM_Cmd(TIM3, ENABLE);
}

void TIM4_PWM_Init(uint16_t pulse, uint8_t channel)
{
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	if (channel > 0 && channel < 5)
	{
		MyGPIO_Init(GPIOB, (GPIO_Pin_6 << (channel - 1)), GPIO_Mode_AF_PP);
	}
	else
	{
		LOG_I("error: invalid PWM port for TIM4");
		return;
	}

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	if (channel == 1)
	{
		TIM_OC1Init(TIM4, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}
	else if (channel == 2)
	{
		TIM_OC2Init(TIM4, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}
	else if (channel == 3)
	{
		TIM_OC3Init(TIM4, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}
	else if (channel == 4)
	{
		TIM_OC4Init(TIM4, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}

	TIM_Cmd(TIM4, ENABLE);
}

void TIMx_Init(TIM_TypeDef *TIMx, uint16_t period, uint16_t prescaler, uint8_t is_pwm, uint16_t pulse, uint16_t channel)
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

	if (is_pwm == 0)
	{
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
	}
	else
	{
		if (TIMx == TIM2)
			TIM2_PWM_Init(pulse, channel);
		else if (TIMx == TIM3)
			TIM3_PWM_Init(pulse, channel);
		else if (TIMx == TIM4)
			TIM4_PWM_Init(pulse, channel);
	}

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
