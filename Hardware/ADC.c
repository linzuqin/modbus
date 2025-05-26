#include "ADC.h"

#define ADC_BUFFER_SIZE 16
#define ADC_CHANNEL     ADC_Channel_0  // PA0
#define ADC_DMA_CHANNEL DMA1_Channel1

static uint16_t adc_buffer[ADC_BUFFER_SIZE];
float adc_light_intensity = 0.0f; // 光照强度变量

// 卡尔曼滤波参数
static float kalman_estimate = 0.0f;
static float kalman_error_estimate = 1.0f;
static const float kalman_q = 0.01f;   // 过程噪声协方差
static const float kalman_r = 2.0f;    // 测量噪声协方差

void ADC_GPIO_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADC_DMA_Config(void) {
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(ADC_DMA_CHANNEL);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adc_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = ADC_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);

    DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);
}

void ADC_Config(void) {
    ADC_InitTypeDef ADC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_CHANNEL, 1, ADC_SampleTime_239Cycles5);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

static void ADC_LightSensor_Init(void) {
    ADC_GPIO_Config();
    ADC_DMA_Config();
    ADC_Config();
}

// 卡尔曼滤波函数
static float KalmanFilter(float measurement) {
    // 预测更新
    kalman_error_estimate += kalman_q;
    // 计算卡尔曼增益
    float kalman_gain = kalman_error_estimate / (kalman_error_estimate + kalman_r);
    // 更新估计值
    kalman_estimate = kalman_estimate + kalman_gain * (measurement - kalman_estimate);
    // 更新估计误差
    kalman_error_estimate = (1 - kalman_gain) * kalman_error_estimate;
    return kalman_estimate;
}

// 获取滤波后的ADC值（先均值，再卡尔曼滤波）
uint16_t ADC_GetFilteredValue(void) {
    uint32_t sum = 0;
    for (int i = 0; i < ADC_BUFFER_SIZE; i++) {
        sum += adc_buffer[i];
    }
    float avg = sum / (float)ADC_BUFFER_SIZE;
    float filtered = KalmanFilter(avg);
    if (filtered < 0) filtered = 0;
    if (filtered > 4095) filtered = 4095;
    return (uint16_t)filtered;
}

// 将ADC值转换为光照强度（假设线性，需根据实际电路调整参数）
float ADC_ConvertToLightIntensity(uint16_t adc_value) {
    float voltage = (adc_value / 4095.0f) * 3.3f; // 12位ADC，3.3V参考
    // 假设光敏电阻与分压电阻组成分压器，返回电压即为光照强度的近似值
    // 可根据实际电路和光敏电阻特性进一步换算
    return voltage;
}

struct rt_thread adc_thread;
uint8_t adc_thread_stack[512];

void ADC_TASK(void*params)
{
	ADC_LightSensor_Init();
	while(1)
	{
			adc_light_intensity = ADC_ConvertToLightIntensity(ADC_GetFilteredValue());
			rt_thread_mdelay(100);
	}
}

int ADC_Task_Init(void)
{
    rt_err_t result = RT_EOK;
    result = rt_thread_init(&adc_thread, "adc", ADC_TASK, RT_NULL, adc_thread_stack, sizeof(adc_thread_stack), 10, 10);
    if(result == RT_EOK)
    {
        rt_thread_startup(&adc_thread);
    }
    return 1;
}

INIT_APP_EXPORT(ADC_Task_Init);