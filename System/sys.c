#include "sys.h"

sys_params_t sys_params = 
{
    .version = "1.0.1"  // ϵͳ�汾
};


int dwt_init(void) 
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	
	return 1;
}

// ��ȡ��ǰ������
uint32_t dwt_read(void) {
    return DWT->CYCCNT;
}

// ����΢�뼶��ʱ
void rt_hw_us_delay(uint32_t us) 
{
    rt_uint32_t ticks;
    rt_uint32_t told, tnow, tcnt = 0;
    rt_uint32_t reload = SysTick->LOAD;

    /* �����ʱ������ tick �� */
    ticks = us * reload / (1000000 / RT_TICK_PER_SECOND);
    /* ��õ�ǰʱ�� */
    told = SysTick->VAL;
    while (1)
    {
        /* ѭ����õ�ǰʱ�䣬ֱ���ﵽָ����ʱ����˳�ѭ�� */
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
