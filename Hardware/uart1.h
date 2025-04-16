#ifndef _UART1_H_
#define _UART1_H_
#include "stm32f10x.h"
#define USART1_RXBUFF_SIZE 256
extern uint8_t USART1_RxData[USART1_RXBUFF_SIZE];

void UART1_Init(uint32_t bound);
void USART1_TASK_START(void);



#endif
