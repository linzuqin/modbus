#ifndef _UART3_H_
#define _UART3_H_
#include "stm32f10x.h"
#define USART3_RXBUFF_SIZE 256
extern uint8_t USART3_RxData[USART3_RXBUFF_SIZE];
extern uint8_t UART3_Count ;
extern uint8_t UART3_RxFlag ;
void UART3_Init(uint32_t bound);
void UART3_SendArray(uint8_t *Array, uint16_t Length);





#endif
