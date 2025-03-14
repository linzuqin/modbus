#ifndef _UART4_H_
#define _UART4_H_
#include "stm32f10x.h"
#define USART4_RXBUFF_SIZE 256

extern uint8_t UART4_RxData[USART4_RXBUFF_SIZE];
extern uint8_t UART4_Count;
extern uint8_t UART4_RxFlag;

void UART4_Init(uint32_t bound);
void UART4_SendArray(uint8_t *Array, uint16_t Length);





#endif
