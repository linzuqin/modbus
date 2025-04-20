#ifndef _GPIO_H_
#define _GPIO_H_
#include "stm32f10x.h" // 标准库头文件
#include "log.h"
#include "rtthread.h" // RT-Thread头文件

void MyGPIO_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef mode);



#endif
