#include "PWM.h"


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

