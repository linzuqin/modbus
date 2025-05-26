#include "AT_Function.h"
static struct rt_timer timer1;
static uint8_t at_msg_buf[AT_MSG_SIZE];

struct rt_thread at_thread;
static uint8_t at_thread_stack[3072];

AT_Device_t AT_Device = {
    .status = AT_HW_INIT,												//默认状态
    .CMD_TABLE = NULL,													//指令列表
    .URC_TABLE = NULL,													//URC指令列表
    .uart_device = &uart_devices[0],						//AT对应的串口
    .msg_buf = at_msg_buf,											//数据缓存数组
	  .init_step = 0,															//初始化步骤
};



void AT_Thread_Entry(void *parameter)
{
    while (1)
    {
        AT_poll(&AT_Device);
        rt_thread_mdelay(10);
    }
}


void timeout1(void *params)
{
	if(AT_Device.status == AT_IDLE)
	{
    AT_Device.status = AT_GET_NTP;
	}
}

int AT_Thread_Init(void)
{
	rt_err_t result = rt_thread_init(&at_thread,
									"at_thread",
									AT_Thread_Entry,
									NULL,
									at_thread_stack,
									sizeof(at_thread_stack),
									15,
									100);
	if (result == RT_EOK)
	{
		rt_thread_startup(&at_thread);
	}
	
	/*初始化定时任务 每分钟获取一次ntp时间*/
	rt_timer_init(&timer1, "timer1",  
							timeout1, 
							RT_NULL,
							60 * 1000, 
							RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&timer1);
  return 0;
}

INIT_APP_EXPORT(AT_Thread_Init);