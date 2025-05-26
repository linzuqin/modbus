#ifndef __UART_H
#define __UART_H
#include "stm32f10x.h"                  // Device header
#include "rtthread.h"
#include <stdbool.h>
#include "GPIO.h"
#include "log.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#define uart1_rx_size    256 // Size of the UART receive buffer
#define uart2_rx_size    256 // Size of the UART receive buffer
#define uart3_rx_size    256 // Size of the UART receive buffer
#define uart4_rx_size    256 // Size of the UART receive buffer
#define uart5_rx_size    256 // Size of the UART receive buffer

#define uart1_tx_size    256 // Size of the UART receive buffer
#define uart2_tx_size    256 // Size of the UART receive buffer
#define uart3_tx_size    256 // Size of the UART receive buffer
#define uart4_tx_size    256 // Size of the UART receive buffer
#define uart5_tx_size    256 // Size of the UART receive buffer

typedef struct uart
{
	USART_TypeDef *port; 	// USART peripheral
	uint32_t baudrate; 		// Baud rate
	uint8_t rx_flag; 		// Flag to indicate if data has been received
	uint16_t rx_size; 		// Size of the received data
	uint16_t rx_max_size;	//单条消息最大接收长度
	uint16_t tx_max_size;	//单条消息最大发送长度
	uint8_t *tx_buffer;		//发送数组 主要用在dma发送
	uint8_t *rx_buffer; 	// Buffer for received data
}uart_device_t;

extern uart_device_t uart_devices[5];

void My_UART_Init(uart_device_t *uart_device);
void UART_SendData(uart_device_t *uart_device, uint8_t *data, uint16_t size); 
void UART_ReadData(uart_device_t *uart_device, uint8_t *buffer, uint16_t *size);
_Bool ai_strstr(const char *haystack, const char *needle);
int8_t UART_DMA_Transmit(uart_device_t *uart_device, uint8_t *pData, uint16_t Size);
uint8_t UART_DMA_TransmitComplete(uart_device_t *uart_device);




#endif