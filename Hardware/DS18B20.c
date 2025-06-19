#include "ds18b20.h"
#include "sys.h"  // 需要实现微秒级延时函数
float temperature;
struct rt_thread ds18b20_pannel;
#define ds18b20_pannel_size 1024
uint8_t ds18b20_pannel_stack[ds18b20_pannel_size];

// 设置数据线为输出模式
static void DS18B20_SetOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

// 设置数据线为输入模式
static void DS18B20_SetInput(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

// 初始化函数
void DS18B20_Init(void)
{
    RCC_APB2PeriphClockCmd(DS18B20_RCC, ENABLE);
    DS18B20_SetOutput();
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN); // 拉高总线
}

// 复位DS18B20
void DS18B20_Reset(void)
{
    DS18B20_SetOutput();
    
    // 拉低总线480us
    GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
    rt_hw_us_delay(480);
    
    // 释放总线
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
    rt_hw_us_delay(60);
    
    // 等待DS18B20响应
    DS18B20_SetInput();
    while(GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN));
    while(!GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN));
    
    // 等待复位结束
    rt_hw_us_delay(420);
}

// 写入一个字节
void DS18B20_WriteByte(uint8_t data)
{
    uint8_t i;
    DS18B20_SetOutput();
    
    for(i = 0; i < 8; i++)
    {
        // 拉低总线开始写时序
        GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
        
        if(data & 0x01) // 写1
        {
            rt_hw_us_delay(2);
            GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
            rt_hw_us_delay(60);
        }
        else // 写0
        {
            rt_hw_us_delay(60);
            GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
            rt_hw_us_delay(2);
        }
        
        data >>= 1;
    }
}

// 读取一个字节
uint8_t DS18B20_ReadByte(void)
{
    uint8_t i, data = 0;
    
    for(i = 0; i < 8; i++)
    {
        DS18B20_SetOutput();
        GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
        rt_hw_us_delay(2);
        
        DS18B20_SetInput();
        rt_hw_us_delay(8);
        
        if(GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN))
        {
            data |= (1 << i);
        }
        
        rt_hw_us_delay(50);
        DS18B20_SetOutput();
        GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
    }
    
    return data;
}

// 开始温度转换
void DS18B20_StartConversion(void)
{
    DS18B20_Reset();
    DS18B20_WriteByte(0xCC); // 跳过ROM
    DS18B20_WriteByte(0x44); // 启动温度转换
}

// 读取温度值(摄氏度)
float DS18B20_ReadTemperature(void)
{
    uint8_t temp_l, temp_h;
    int16_t temp;
    float temperature;
    
    DS18B20_StartConversion();
    
    // 等待转换完成(最大750ms)
    DS18B20_SetInput();
    while(!GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN));
    
    DS18B20_Reset();
    DS18B20_WriteByte(0xCC); // 跳过ROM
    DS18B20_WriteByte(0xBE); // 读取暂存器
    
    temp_l = DS18B20_ReadByte(); // LSB
    temp_h = DS18B20_ReadByte(); // MSB
    
    temp = (temp_h << 8) | temp_l;
    temperature = temp * 0.0625f; // 12位分辨率
    
    return temperature;
}

void ds18b20_pannel_entry(void *params)
{
    while (1)
    {
        temperature = DS18B20_ReadTemperature();
        rt_thread_mdelay(1000); // 每秒读取一次
    }
}

int ds18b20_start(void)
{
	DS18B20_Init();
	rt_thread_init(&ds18b20_pannel , "ds18b20_pannel", 
        ds18b20_pannel_entry, 
        RT_NULL, ds18b20_pannel_stack, ds18b20_pannel_size, 
        10, 100);
	
	rt_thread_startup(&ds18b20_pannel);
	return 1;
}

//INIT_APP_EXPORT(ds18b20_start);
