#include "GPIO.h" 

void MyGPIO_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef mode)
{
    // 启用GPIO时钟
    if (GPIOx == GPIOA)
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    else if (GPIOx == GPIOB)
        RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    else if (GPIOx == GPIOC)
        RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    else if (GPIOx == GPIOD)
        RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
    else if (GPIOx == GPIOE)
        RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
    else
    {
        LOG_I("error: invalid GPIO port"); // 无效的GPIO端口
        return; // 无效的GPIO端口
    }

    // 配置GPIO引脚
    uint32_t pin_pos = 0;
    while ((GPIO_Pin >> pin_pos) != 1)
        pin_pos++;

    if (pin_pos < 8)
    {
        GPIOx->CRL &= ~(0xF << (pin_pos * 4)); // 清除原有设置
        GPIOx->CRL |= (mode << (pin_pos * 4)); // 设置模式
    }
    else
    {
        pin_pos -= 8;
        GPIOx->CRH &= ~(0xF << (pin_pos * 4)); // 清除原有设置
        GPIOx->CRH |= (mode << (pin_pos * 4)); // 设置模式
    }
}
