#ifndef _GPIO_H_
#define _GPIO_H_
#include "stm32f10x.h" // 标准库头文件
#include "log.h"
#include "rtthread.h" // RT-Thread头文件

#define IO_NUM  1
typedef struct
{
    GPIO_TypeDef *Port;                 //引脚对应的端口号
    uint16_t Pin;                       //引脚编号
    GPIOMode_TypeDef mode;              //模式
    uint8_t output;                     //输出状态
    uint8_t input;                      //输入状态
}MY_GPIO_t;

void MyGPIO_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef mode);
void io_tasl_init(void);



#endif
