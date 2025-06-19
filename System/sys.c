#include "sys.h"

sys_params_t sys_params = 
{
    .version = "1.0.1"  // 系统版本
};


int dwt_init(void) 
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	
	return 1;
}

// 获取当前周期数
uint32_t dwt_read(void) {
    return DWT->CYCCNT;
}

// 计算微秒级延时
void rt_hw_us_delay(uint32_t us) 
{
    rt_uint32_t ticks;
    rt_uint32_t told, tnow, tcnt = 0;
    rt_uint32_t reload = SysTick->LOAD;

    /* 获得延时经过的 tick 数 */
    ticks = us * reload / (1000000 / RT_TICK_PER_SECOND);
    /* 获得当前时间 */
    told = SysTick->VAL;
    while (1)
    {
        /* 循环获得当前时间，直到达到指定的时间后退出循环 */
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

INIT_APP_EXPORT(dwt_init);
