#include "GPIO.h" 

void MyGPIO_Init(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIOMode_TypeDef mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;

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
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = mode==1?GPIO_Mode_Out_PP : GPIO_Mode_IN_FLOATING;  // 默认推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 默认速度

    GPIO_Init(GPIOx, &GPIO_InitStructure);
}
