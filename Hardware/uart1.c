#include "uart1.h"
#include "rtthread.h"
#include "log.h"
#include "string.h"
#include "main.h"

#define USART1_RXBUFF_SIZE 256
uint8_t USART1_RxData[USART1_RXBUFF_SIZE];
uint8_t UART1_Count = 0;
uint8_t UART1_RxFlag = 0;
extern struct rt_messagequeue  modbus_rx_mq;								
//extern struct rt_messagequeue ymodem_mq;/* 消息队列控制块 */
extern ota_t ota;

struct rt_thread	u1_tx;
static uint8_t u1_tx_stack[1024];			

void UART1_DMA_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &USART1_RxData[2];
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = USART1_RXBUFF_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA1_Channel5, ENABLE);
}

void UART1_Init(uint32_t bound)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // TX Pin
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // RX Pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // Enable USART1 Receive interrupt
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    USART_ITConfig(USART1,USART_IT_TC,DISABLE);  

    // Enable USART1 global interrupt
    NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStruct);

    UART1_DMA_Init();
    USART_Cmd(USART1, ENABLE);
    LOG_I("UART1 Init OK\r\n");
}


void UART1_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void UART1_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		UART1_SendByte(Array[i]);
	}
}

void UART1_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		UART1_SendByte(String[i]);
	}
}

uint32_t UART1_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void UART1_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		UART1_SendByte(Number / UART1_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)
	{
		volatile uint8_t clear;
		clear = USART1->SR;
		clear = USART1->DR;
    USART_ClearFlag(USART1, USART_FLAG_IDLE); 
		
		UART1_RxFlag = 1;
		USART1_RxData[0] = 1;
		USART1_RxData[1] = USART1_RXBUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);
//		if(ota.ota_flag == 1)
//		{
//					rt_mq_send(&ymodem_mq , USART1_RxData ,USART1_RxData[1] + 2);
//		}
//		else
//		{
//			#if MODBUS_SLAVE_UART1
//			rt_mq_send(&modbus_rx_mq , USART1_RxData ,USART1_RxData[1] + 2);
//			#endif
//		}

		DMA_Cmd(DMA1_Channel5,DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel5, USART1_RXBUFF_SIZE);
		DMA_Cmd(DMA1_Channel5, ENABLE);		
	}
}

void u1_tx_pannel(void* params)
{
	while(1)
	{


		rt_thread_mdelay(10);
	}

}

void USART1_TASK_START(void)
{
	rt_err_t result = 0;
	

	
	result = rt_thread_init(&u1_tx , "u1_tx" ,u1_tx_pannel , RT_NULL ,  u1_tx_stack , sizeof(u1_tx_stack) , 22 , 100);
	if(result == RT_EOK)
	{
		rt_thread_startup(&u1_tx);
	}
}
