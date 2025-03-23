#include "UART3.h"
#include "main.h"

uint8_t USART3_RxData[USART3_RXBUFF_SIZE];
uint8_t UART3_Count = 0;
uint8_t UART3_RxFlag = 0;
extern struct rt_messagequeue  modbus_rx_mq;								

static void UART3_DMA_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&USART3_RxData[2];
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = USART3_RXBUFF_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA1_Channel3, ENABLE);
}

void UART3_Init(uint32_t bound)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // TX Pin
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // RX Pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    // Enable USART3 Receive interrupt
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
    USART_ITConfig(USART3,USART_IT_TC,DISABLE);  

    // Enable USART3 global interrupt
    NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStruct);

    UART3_DMA_Init();
    USART_Cmd(USART3, ENABLE);

    LOG_I("UART3 Init OK\r\n");
}


void UART3_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

void UART3_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		UART3_SendByte(Array[i]);
	}
}

void UART3_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		UART3_SendByte(String[i]);
	}
}

uint32_t UART3_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void UART3_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		UART3_SendByte(Number / UART3_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3,USART_IT_IDLE) != RESET)
	{
		volatile uint8_t clear;
		clear = USART3->SR;
		clear = USART3->DR;
		USART_ClearFlag(USART3, USART_FLAG_IDLE); 
		
		UART3_RxFlag = 1;
		//UART3_Count = USART3_RXBUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);
		USART3_RxData[0] = 3;
		USART3_RxData[1] = USART3_RXBUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);

		#if MODBUS_SLAVE_UART3
		rt_mq_send(&modbus_rx_mq , USART3_RxData ,USART3_RxData[1] + 2);
		#endif
		
		DMA_Cmd(DMA1_Channel3,DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel3, USART3_RXBUFF_SIZE);
		DMA_Cmd(DMA1_Channel3, ENABLE);		
	}
}


