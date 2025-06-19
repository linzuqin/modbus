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

#define uart1_rx_size    2048 // Size of the UART receive buffer
#define uart2_rx_size    256 // Size of the UART receive buffer
#define uart3_rx_size    256 // Size of the UART receive buffer
#define uart4_rx_size    256 // Size of the UART receive buffer
#define uart5_rx_size    256 // Size of the UART receive buffer

#define uart1_tx_size    256 // Size of the UART receive buffer
#define uart2_tx_size    256 // Size of the UART receive buffer
#define uart3_tx_size    256 // Size of the UART receive buffer
#define uart4_tx_size    256 // Size of the UART receive buffer
#define uart5_tx_size    256 // Size of the UART receive buffer

typedef struct {
    uint8_t* buffer;     // 缓冲区指针
    uint16_t size;       // 缓冲区总大小
    volatile uint16_t head; // 写入位置（需volatile保证多线程安全）
    volatile uint16_t tail; // 读取位置
    uint16_t overwrite;  // 是否允许覆盖旧数据
} ring_buf_t;

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
	
	DMA_Channel_TypeDef * rx_dma_channel;
	DMA_Channel_TypeDef * tx_dma_channel;
	
	ring_buf_t *ring_buf;					//串口环形缓冲区

}uart_device_t;

extern uint8_t uart1_rx_buf[uart1_rx_size];
extern uint8_t uart2_rx_buf[uart2_rx_size];
extern uint8_t uart3_rx_buf[uart3_rx_size];

extern uart_device_t uart_devices[5];

void My_UART_Init(uart_device_t *uart_device);
void UART_SendData(uart_device_t *uart_device, uint8_t *data, uint16_t size); 
uint8_t UART_ReadData(uart_device_t *uart_device, uint8_t *buffer, uint16_t size); 
_Bool ai_strstr(const char *haystack, const char *needle);
int8_t UART_DMA_Transmit(uart_device_t *uart_device, uint8_t *pData, uint16_t Size);
uint8_t UART_DMA_TransmitComplete(uart_device_t *uart_device);
const void* binary_strstr(const void* haystack, size_t h_len, 
                         const void* needle, size_t n_len);
void uartbuf_clear(uart_device_t *uart_device);
uint16_t ringbuf_get(ring_buf_t* rb, uint8_t* dest, uint16_t len);
void ring_reset(ring_buf_t* rb);

// 快速计算缓冲区剩余空间（宏避免函数调用开销）
#define RINGBUF_FREE(rb) ((rb)->tail > (rb)->head ? (rb)->tail - (rb)->head - 1 : (rb)->size + (rb)->tail - (rb)->head - 1)

// 快速计算已用数据量
#define RINGBUF_USED(rb) ((rb)->head >= (rb)->tail ? (rb)->head - (rb)->tail : (rb)->size + (rb)->head - (rb)->tail)

#endif