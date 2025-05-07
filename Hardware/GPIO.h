#ifndef _GPIO_H_
#define _GPIO_H_
#include "stm32f10x.h" // 标准库头文件
#include "log.h"
#include "rtthread.h" // RT-Thread头文件

#define OUTPUT1_PORT GPIOA
#define OUTPUT1_PIN  GPIO_Pin_0
#define OUTPUT1_Write(x) (x ? (OUTPUT1_PORT->BSRR = OUTPUT1_PIN) : (OUTPUT1_PORT->BRR = OUTPUT1_PIN)) // 输出写入函数
#define OUTPUT1_Read ((OUTPUT1_PORT->ODR & OUTPUT1_PIN) ? 1 : 0) // 输出读取函数

#define INPUT1_PORT GPIOA
#define INPUT1_PIN  GPIO_Pin_0
#define INPUT1_Read ((INPUT1_PORT->IDR & INPUT1_PIN) ? 1 : 0) // 使用寄存器方式读取输入

void MyGPIO_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef mode);



#endif
