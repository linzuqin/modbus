#include "AT_Function.h"
static struct rt_timer timer1;
static uint8_t at_msg_buf[AT_MSG_SIZE];

struct rt_thread at_thread;
static uint8_t at_thread_stack[3072];

AT_Device_t AT_Device = {
    .status = AT_HW_INIT,												//Ĭ��״̬
    .CMD_TABLE = NULL,													//ָ���б�
    .URC_TABLE = NULL,													//URCָ���б�
    .uart_device = &uart_devices[0],						//AT��Ӧ�Ĵ���
    .msg_buf = at_msg_buf,											//���ݻ�������
	  .init_step = 0,															//��ʼ������
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
	
	/*��ʼ����ʱ���� ÿ���ӻ�ȡһ��ntpʱ��*/
	rt_timer_init(&timer1, "timer1",  
							timeout1, 
							RT_NULL,
							60 * 1000, 
							RT_TIMER_FLAG_PERIODIC);
	rt_timer_start(&timer1);
  return 0;
}

INIT_APP_EXPORT(AT_Thread_Init);