#include "stm32f10x.h"                  // Device header

void TIM3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 时钟使能

	TIM_TimeBaseStructure.TIM_Period = 1000; // 自动重装载寄存器周期的值1/CK_CNT=1us,1000x1us=1ms
	TIM_TimeBaseStructure.TIM_Prescaler = 71; // 设置用来作为TIMx时钟频率除数的预分频值CK_CNT=CK_INT/(71+1)=1MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频因子为0
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // 根据指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // 使能指定的TIM3中断, 允许更新中断

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置NVIC中断分组2:2位抢占优先级，2位响应优先级

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; // TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; // 从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure); // 初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE); // 使能TIMx
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // 检查TIM3更新中断发生与否
	{
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除TIMx更新中断标志
	}
}