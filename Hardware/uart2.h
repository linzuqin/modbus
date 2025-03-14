#ifndef _UART2_H_
#define _UART2_H_
#include "stm32f10x.h"


#define DATA_OK             0
#define RECEIVE_TIMEOUT     1
#define CRC_ERROR           2
#define FUNCTION_CODE_ERROR 3

void UART2_Init(uint32_t bound);
void USART2_TASK_START(void);





#endif
