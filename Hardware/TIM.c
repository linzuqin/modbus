#include "stm32f10x.h"                  // Device header

void TIM3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = 1000; // �Զ���װ�ؼĴ������ڵ�ֵ1/CK_CNT=1us,1000x1us=1ms
	TIM_TimeBaseStructure.TIM_Prescaler = 71; // ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��ƵֵCK_CNT=CK_INT/(71+1)=1MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // ʱ�ӷ�Ƶ����Ϊ0
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // ����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // ʹ��ָ����TIM3�ж�, ��������ж�

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // ����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; // TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // ��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; // �����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure); // ��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE); // ʹ��TIMx
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // ���TIM3�����жϷ������
	{
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ���TIMx�����жϱ�־
	}
}