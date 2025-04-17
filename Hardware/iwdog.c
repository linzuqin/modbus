#include "iwdog.h"

#define IWDOG_PANNEL_SIZE   256
static struct rt_thread IWDOG_PANNEL;
static uint8_t IWDOG_PANNEL_STACK[IWDOG_PANNEL_SIZE];

// Configurable stack size for the watchdog thread
#define WATCHDOG_THREAD_STACK_SIZE 256

void IWDG_Init(uint8_t prescaler, uint16_t reload)
{
    // 启用写访问
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    // 设置预分频器
    IWDG_SetPrescaler(prescaler);

    // 设置重装载值
    IWDG_SetReload(reload);

    // 重新加载计数器
    IWDG_ReloadCounter();

    // 启动独立看门狗
    IWDG_Enable();
}

// 喂狗函数
void IWDG_Feed(void)
{
    // 重新加载计数器
    IWDG_ReloadCounter();
	
	//LOG_I("Feed IWDOG....");
}
