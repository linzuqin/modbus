#include "AT_Function.h"
static struct rt_timer timer1;
static struct rt_timer timer2;

static uint8_t at_msg_buf[AT_MSG_SIZE];

struct rt_thread at_thread;
uint8_t at_thread_stack[10240];

AT_Device_t AT_Device = {
    .status = AT_HW_INIT,												
    .CMD_TABLE = NULL,													
    .URC_TABLE = NULL,													
    .uart_device = &AT_DEFAULT_UART_DEVICE,						
    .msg_buf = at_msg_buf,											
	  .init_step = 0,															
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

void timeout2(void *params)
{
	if(AT_Device.status == AT_IDLE)
	{
    AT_Device.status = AT_UPDATA;
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
									10,
									100);
	if (result == RT_EOK)
	{
		rt_thread_startup(&at_thread);
	}
	
	rt_timer_init(&timer1, "timer1",  
							timeout1, 
							RT_NULL,
							60 * 1000, 
							RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&timer1);

	rt_timer_init(&timer2, "timer2",  
							timeout2, 
							RT_NULL,
							3 * 1000, 
							RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&timer2);
  return 0;
}

INIT_APP_EXPORT(AT_Thread_Init);