#ifndef _SPI_H_
#define _SPI_H_
#include "stm32f10x.h" // Replace with the appropriate header file for your STM32 series
#include "rtthread.h"
#include "log.h"
#include "GPIO.h"

#define SPI1_rx_SIZE    64
#define SPI2_rx_SIZE    64

void My_SPI_Init(SPI_TypeDef *SPIx);
void SPI_Write(SPI_TypeDef *SPIx, uint8_t *txBuffer, uint8_t *rxBuffer , uint16_t size);






#endif
