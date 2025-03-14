#include "UART5.h"
#include "main.h"

uint8_t UART5_RxData[USART5_RXBUFF_SIZE];
uint8_t UART5_Count = 0;
uint8_t UART5_RxFlag = 0;
extern struct rt_messagequeue  modbus_rx_mq;								//消息队列句柄

void UART5_Init(uint32_t bound)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
		 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; // TX Pin
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // RX Pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART5, &USART_InitStructure);

    // Enable UART5 Receive interrupt
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
    USART_ITConfig(UART5,USART_IT_TC,DISABLE);  

    // Enable UART5 global interrupt
    NVIC_InitTypeDef NVIC_InitStruct;
		NVIC_InitStruct.NVIC_IRQChannel=UART5_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
		NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
		NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
		NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(UART5, ENABLE);
		
    GPIO_InitStructure.GPIO_Pin = TX_LED_PIN | RX_LED_PIN; // power pin as intput
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(TX_LED_PORT, &GPIO_InitStructure);


    LOG_I("UART5 Init OK\r\n");
}


void UART5_SendByte(uint8_t Byte)
{
	USART_SendData(UART5, Byte);
	while (USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
}

void UART5_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		UART5_SendByte(Array[i]);
	}
}

void UART5_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		UART5_SendByte(String[i]);
	}
}

uint32_t UART5_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void UART5_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		UART5_SendByte(Number / UART5_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void UART5_IRQHandler(void)
{
    volatile uint8_t clear;
	if(USART_GetITStatus(UART5,USART_IT_RXNE)!=RESET)     
	{
		UART5_RxData[2+UART5_Count++]=USART_ReceiveData(UART5);  
		if(UART5_Count > USART5_RXBUFF_SIZE - 2) UART5_Count = 0;
		USART_ClearITPendingBit(UART5, USART_IT_RXNE);				
		
	}
	else if(USART_GetITStatus(UART5,USART_IT_IDLE)!=RESET)  
	{
		UART5_RxFlag = 1;   
		clear = UART5->SR; 
		clear = UART5->DR;
		UART5_RxData[0] = 5;
		UART5_RxData[1] = UART5_Count;
		
		#if MODBUS_SLAVE_UART5

			rt_mq_send(&modbus_rx_mq , UART5_RxData ,UART5_RxData[1] + 2);
			rt_memset(UART5_RxData , 0 ,USART5_RXBUFF_SIZE );
			UART5_Count = 0;
		
		#endif
		USART_ClearITPendingBit(UART5, USART_IT_IDLE);				 
	}

}
	
	


