#ifndef __UART_H
#define __UART_H

#define uart_tx_size    256 // Size of the UART transmit buffer
#define uart_rx_size    256 // Size of the UART receive buffer



typedef struct uart
{
    uint8_t rx_flag; // Flag to indicate if data has been received
    uint16_t rx_size; // Size of the received data
    uint8_t tx_flag; // Flag to indicate if data has been transmitted
    uint8_t rx_buffer[uart_rx_size]; // Buffer for received data
    uint8_t tx_buffer[uart_tx_size]; // Buffer for transmitted data
}uart_device_t;

void UART_Init(USART_TypeDef *USARTx , uint32_t baudrate);
void UART_SendData(USART_TypeDef *USARTx, uint8_t *data, uint16_t size);




#endif