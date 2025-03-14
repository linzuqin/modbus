#ifndef _UART5_H_
#define _UART5_H_
#include "stm32f10x.h"
#define USART5_RXBUFF_SIZE 256

#define TX_LED_PIN	GPIO_Pin_3
#define RX_LED_PIN	GPIO_Pin_4

#define TX_LED_PORT	GPIOB
#define RX_LED_PORT	TX_LED_PORT

#define TX_LED(x)		GPIO_WriteBit(TX_LED_PORT , TX_LED_PIN , (BitAction)x)
#define RX_LED(x)		GPIO_WriteBit(RX_LED_PORT , RX_LED_PIN , (BitAction)x)

extern uint8_t UART5_RxData[USART5_RXBUFF_SIZE];
extern uint8_t UART5_Count;
extern uint8_t UART5_RxFlag;

void UART5_Init(uint32_t bound);
void UART5_SendArray(uint8_t *Array, uint16_t Length);





#endif
