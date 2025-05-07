#include "GPIO.h" 

void MyGPIO_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef mode)
{
    // 启用GPIO时钟
    if (GPIOx == GPIOA)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    else if (GPIOx == GPIOB)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    else if (GPIOx == GPIOC)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    else if (GPIOx == GPIOD)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    else if (GPIOx == GPIOE)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    else
    {
        LOG_I("error: invalid GPIO port"); // 无效的GPIO端口
        return; // 无效的GPIO端口
    }

    // 配置GPIO引脚
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
    GPIO_InitStruct.GPIO_Mode = mode;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOx, &GPIO_InitStruct); // 使用标准库函数初始化GPIO
}
