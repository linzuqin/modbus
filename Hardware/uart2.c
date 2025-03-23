#include "UART2.h"
#include "main.h"

#define USART2_RXBUFF_SIZE 256
uint8_t USART2_RxData[USART2_RXBUFF_SIZE];
uint8_t UART2_Count = 0;
uint8_t UART2_RxFlag = 0;

#define UART2_PANNEL_STACK_SIZE 1024
static struct rt_thread UART2_PANNEL;
static char UART2_PANNEL_STACK[UART2_PANNEL_STACK_SIZE];
extern struct rt_messagequeue  modbus_rx_mq;								


void UART2_TASK(void*params);


static void UART2_DMA_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &USART2_RxData[2];
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = USART2_RXBUFF_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA1_Channel6, ENABLE);
}

void UART2_Init(uint32_t bound)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // TX Pin
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; // RX Pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    // Enable USART2 Receive interrupt
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_ITConfig(USART2,USART_IT_TC,DISABLE);  

    // Enable USART2 global interrupt
    NVIC_InitTypeDef NVIC_InitStruct;
		NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
		NVIC_Init(&NVIC_InitStruct);

    UART2_DMA_Init();
    USART_Cmd(USART2, ENABLE);

    LOG_I("UART2 Init OK\r\n");

}

void USART2_TASK_START(void)
{
    rt_err_t result = 0;

    result = rt_thread_init(&UART2_PANNEL , "UART2_TASK" , UART2_TASK , RT_NULL , &UART2_PANNEL_STACK[0] , sizeof(UART2_PANNEL_STACK) , 22 , 100);
		if(result == RT_EOK)
    {
        rt_thread_startup(&UART2_PANNEL);
        LOG_I("UART2 TASK START OK\r\n");
    }
    else{
        LOG_E("UART2 TASK START FAIL\r\n");
    }
}

void UART2_SendByte(uint8_t Byte)
{
	USART_SendData(USART2, Byte);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void UART2_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		UART2_SendByte(Array[i]);
	}
}

void UART2_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		UART2_SendByte(String[i]);
	}
}

uint32_t UART2_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void UART2_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		UART2_SendByte(Number / UART2_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		volatile uint8_t clear;
		clear = USART2->SR;
		clear = USART2->DR;
		USART_ClearFlag(USART2, USART_FLAG_IDLE); 
		
		UART2_RxFlag = 1;
		USART2_RxData[0] = 2;
		USART2_RxData[1] = USART2_RXBUFF_SIZE - DMA_GetCurrDataCounter(DMA1_Channel6);
		
		#if MODBUS_SLAVE_UART2
		rt_mq_send(&modbus_rx_mq , USART2_RxData ,USART2_RxData[1] + 2);
		#endif
		
		DMA_Cmd(DMA1_Channel6, DISABLE);
		DMA_SetCurrDataCounter(DMA1_Channel6, USART2_RXBUFF_SIZE);
		memset(USART2_RxData , 0 , USART2_RXBUFF_SIZE);

		DMA_Cmd(DMA1_Channel6, ENABLE);		
	}
}

void UART2_TASK(void * params)
{
	while(1)
	{


		rt_thread_mdelay(10);
	}
}
