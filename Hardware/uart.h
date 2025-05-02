#ifndef __UART_H
#define __UART_H
#include "main.h"

typedef struct uart
{
	USART_TypeDef *port; 	// USART peripheral
	uint32_t baudrate; 	// Baud rate
	uint8_t rx_flag; 		// Flag to indicate if data has been received
	uint16_t rx_size; 		// Size of the received data
	uint8_t *rx_buffer; 	// Buffer for received data
}uart_device_t;

extern uart_device_t uart1_device;
extern uart_device_t uart2_device;
extern uart_device_t uart3_device;
extern uart_device_t uart4_device;
extern uart_device_t uart5_device;

void My_UART_Init(uart_device_t *uart_device);
void UART_SendData(USART_TypeDef *USARTx, uint8_t *data, uint16_t size);
void UART_ReadData(uart_device_t *uart_device, uint8_t *buffer, uint16_t *size);




#endif