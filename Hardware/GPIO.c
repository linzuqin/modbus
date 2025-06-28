#include "GPIO.h" 

MY_GPIO_t my_io[IO_NUM];

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


void my_io_init(void)
{
    uint8_t i = 0;
    for(i = 0; i < IO_NUM;i++)
    {
        MyGPIO_Init(my_io[i].Port , my_io[i].Pin , my_io[i].mode);
    }
}

void IO_TASK(void *params)
{
    while(1)
    {
        for(uint8_t i = 0;i<IO_NUM;i++)
        {
            if( (my_io[i].mode & 0x10) == 1)               //输出模式
            {
                GPIO_WriteBit(my_io[i].Port , my_io[i].Pin , my_io[i].output);
            }
            else
            {
                my_io[i].input = GPIO_ReadInputDataBit(my_io[i].Port , my_io[i].Pin);
            }
        }

        rt_thread_mdelay(100);
    }
}

struct rt_thread IO_PANNEL;
uint8_t io_stack[256];

void io_tasl_init(void)
{
    rt_err_t result = RT_EOK;
    result = rt_thread_init(&IO_PANNEL , "io_task" , IO_TASK , RT_NULL , io_stack , sizeof(io_stack) , 22 , 100);
    if(result == RT_EOK)
    {
        rt_thread_startup(&IO_PANNEL);
    }
}

