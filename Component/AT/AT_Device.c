#include "AT_Device.h"

#define AT_THREAD_STACK_SIZE 1024
#define AT_URC_THREAD_STACK_SIZE	1024
#define AT_INIT_THREAD_STACK_SIZE 1024

static struct rt_thread AT_Thread;
static uint8_t AT_ThreadStack[AT_THREAD_STACK_SIZE];

static struct rt_thread AT_Init_Thread;
static uint8_t AT_Init_ThreadStack[AT_INIT_THREAD_STACK_SIZE];

static struct rt_thread AT_URC_Thread;
static uint8_t AT_URC_ThreadStack[AT_URC_THREAD_STACK_SIZE];

AT_Device_t AT_Device;

/*AT DEVICE CMD*/
AT_Command_t AT_Cmd[AT_COMMAND_ARRAY_SIZE] = {
    {"AT\r\n",          "OK",   100 , NULL , NULL},
    {"AT+GMR\r\n",      "OK",   100 , NULL , NULL},
    {"AT+RST\r\n",      "OK",   200 , NULL , NULL},
    {"AT+CWMODE?\r\n",  "OK",   100 , NULL , NULL},
    {"AT+CWJAP?\r\n",   "OK",   200 , NULL , NULL},
    {"AT+CIPSTART\r\n", "OK",   500 , NULL , NULL},
    {"AT+CIPSEND\r\n",  ">",    500 , NULL , NULL},
    {"AT+CIPCLOSE\r\n", "OK",   200 , NULL , NULL},
    {"AT+CWQAP\r\n`",   "OK",   200 , NULL , NULL}
};

AT_Command_t AT_Init_Cmd[AT_COMMAND_ARRAY_SIZE] ={
    {"AT\r\n", "OK", 1000 , NULL , NULL}
    //{"AT+RST\r\n", "OK", 2000 , NULL , NULL}
};

AT_URC_t AT_URC_Msg[AT_COMMAND_ARRAY_SIZE] = {
    {"WIFI GOT IP",             NULL},
    {"WIFI DISCONNECT",         NULL},
    {"CLOSED",                  NULL},
    {"SEND OK",                 NULL},
    {"ERROR",                   NULL},
    {"NO Service",              NULL}
};

void AT_Init(void *params)
{
    static uint8_t *step = &AT_Device.init_step;
    AT_Device.status = AT_DISCONNECT;
    static AT_Command_t *cmd = AT_Init_Cmd;
    while(1)
    {
       if (AT_SendCmd(&AT_Device , cmd[*step].cmd, cmd[*step].response, cmd[*step].timeout) != 0)
       {
            if(cmd[*step].ack_err_response != NULL)
            {
                cmd[*step].ack_err_response();
            }
       }
       else
       {
            if(cmd[*step].ack_right_response != NULL)
            {
                cmd[*step].ack_right_response();
            }
            (*step) ++;

            if (*step >= AT_COMMAND_ARRAY_SIZE)
            {

                AT_Device.status = AT_CONNECT;
                LOG_I("AT_Init finish\n");

                break;
            }
       }

        rt_thread_mdelay(1000);
    }
}

void AT_TASK(void *params)
{
    while(1)
    {
        if(AT_Device.status == AT_DISCONNECT)
        {
            if(AT_Init_Thread.stat == RT_THREAD_CLOSE)
            {
//                rt_err_t result;
//                result = rt_thread_init(&AT_Init_Thread, "AT_Init", AT_Init, RT_NULL, &AT_Init_ThreadStack[0],AT_THREAD_STACK_SIZE, 10, 10);
//                if (result != RT_EOK)
//                {
//                        rt_kprintf("AT_Init thread create failed\n");
//                        return;
//                }
//                result = rt_thread_startup(&AT_Init_Thread);
//                if (result != RT_EOK)
//                {
//                    rt_kprintf("AT_Init thread startup failed\n");
//                    return;
//                }
            }
        }
        else {
            switch(AT_Device.status)
            {
                case AT_CONNECT:AT_Device.status = AT_SEND;break;
                case AT_IDEL:
                {

                }
                break;
                case AT_SEND:

                break;
            }
        }
        rt_thread_mdelay(1000);
    }
}

void AT_URC(void *params)
{
	while(1)
	{
        for(uint8_t i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
        {
            if(strstr((char *)&AT_Device.rx_buf[AT_DATA_START_BIT], AT_URC_Msg[i].urc_msg) != NULL)
            {
                if(AT_URC_Msg[i].response != NULL)
                {
                    AT_URC_Msg[i].response(&AT_Device);
                }
            }
        }        
		rt_thread_mdelay(10);
	}
}


void AT_START(void)
{
    rt_err_t result;
    at_device_register(AT_PORT , AT_BAUD , &AT_Device , uart1_device.rx_buffer);
    result = rt_thread_init(&AT_Init_Thread, "AT_Init", AT_Init, RT_NULL, &AT_Init_ThreadStack[0], AT_INIT_THREAD_STACK_SIZE, 10, 10);
    if (result != RT_EOK)
    {
        rt_kprintf("AT_Init thread create failed\n");
        return;
    }
    result = rt_thread_startup(&AT_Init_Thread);
    if (result != RT_EOK)
    {
        rt_kprintf("AT_Init thread startup failed\n");
        return;
    }

    /*AT URC PANNEL*/
    result = rt_thread_init(&AT_URC_Thread, "AT_URC", AT_Init, RT_NULL, &AT_URC_ThreadStack[0], AT_URC_THREAD_STACK_SIZE, 10, 10);
    if (result != RT_EOK)
    {
        rt_kprintf("AT_URC thread create failed\n");
        return;
    }
    result = rt_thread_startup(&AT_URC_Thread);
    if (result != RT_EOK)
    {
        rt_kprintf("AT_URC thread startup failed\n");
        return;
    }

    rt_thread_init(&AT_Thread, "AT", AT_TASK, RT_NULL, &AT_ThreadStack[0], AT_THREAD_STACK_SIZE, 10, 10);
    if (result != RT_EOK)
    {
        rt_kprintf("AT thread create failed\n");
        return;
    }
		rt_thread_startup(&AT_Thread);

}
