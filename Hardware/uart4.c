#include "UART4.h"
#include "main.h"

uint8_t UART4_RxData[USART4_RXBUFF_SIZE];
uint8_t UART4_Count = 0;
uint8_t UART4_RxFlag = 0;
extern struct rt_messagequeue  modbus_rx_mq;								//消息队列句柄

void UART4_DMA_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2,ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&UART4->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&UART4_RxData[2];
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = USART4_RXBUFF_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);
	
  USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA2_Channel3, ENABLE);
}

void UART4_Init(uint32_t bound)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // TX Pin
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // RX Pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure);

    // Enable UART4 Receive interrupt
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
    USART_ITConfig(UART4,USART_IT_TC,DISABLE);  

    // Enable UART4 global interrupt
    NVIC_InitTypeDef NVIC_InitStruct;
		NVIC_InitStruct.NVIC_IRQChannel=UART4_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
		NVIC_Init(&NVIC_InitStruct);

    UART4_DMA_Init();
    USART_Cmd(UART4, ENABLE);

    LOG_I("UART4 Init OK\r\n");
}


void UART4_SendByte(uint8_t Byte)
{
	USART_SendData(UART4, Byte);
	while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
}

void UART4_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		UART4_SendByte(Array[i]);
	}
}

void UART4_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		UART4_SendByte(String[i]);
	}
}

uint32_t UART4_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void UART4_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		UART4_SendByte(Number / UART4_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void UART4_IRQHandler(void)//适配中断服务函数
{
		if(USART_GetITStatus(UART4,USART_IT_IDLE) != RESET)
		{
			volatile uint8_t clear;
			clear = UART4->SR;
			clear = UART4->DR;
			USART_ClearFlag(UART4, USART_FLAG_IDLE); 

			UART4_RxFlag = 1;
			UART4_RxData[0] = 4;
			UART4_RxData[1] = 256 - DMA_GetCurrDataCounter(DMA2_Channel3);
			
			#if MODBUS_SLAVE_UART4
			rt_mq_send(&modbus_rx_mq , UART4_RxData ,UART4_RxData[1] + 2);
			#endif
			
			DMA_Cmd(DMA2_Channel3,DISABLE);
			DMA_SetCurrDataCounter(DMA2_Channel3, 256);
			DMA_Cmd(DMA2_Channel3, ENABLE);		
		}
}

