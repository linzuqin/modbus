#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f10x.h"
extern float temperature;

#define TEMP_HIGH   30

// DS18B20数据引脚定义
#define DS18B20_PIN      GPIO_Pin_0
#define DS18B20_PORT     GPIOB
#define DS18B20_RCC      RCC_APB2Periph_GPIOB

// 函数声明
void DS18B20_Reset(void);
void DS18B20_WriteByte(uint8_t data);
uint8_t DS18B20_ReadByte(void);
void DS18B20_StartConversion(void);
float DS18B20_ReadTemperature(void);

#endif