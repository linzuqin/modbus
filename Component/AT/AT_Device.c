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

/*ESP DEVICE CMD*/
AT_Command_t ESP_Command[AT_COMMAND_ARRAY_SIZE] = {
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

AT_Command_t CAT_Command[AT_COMMAND_ARRAY_SIZE] = {
    {"AT\r\n",          "OK",   100 , NULL , NULL},
    {"AT+GMR\r\n",      "OK",   100 , NULL , NULL},
    {"AT+RST\r\n",      "OK",   200 , NULL , NULL},
    {"AT+CWMODE?\r\n",  "OK",   100 , NULL , NULL},
    {"AT+CWJAP?\r\n",   "OK",   200 , NULL , NULL},
    {"AT+CIPSTART\r\n", "OK",   500 , NULL , NULL},
    {"AT+CIPSEND\r\n",  ">",    500 , NULL , NULL},
    {"AT+CIPCLOSE\r\n", "OK",   200 , NULL , NULL},
    {"AT+CWQAP\r\n",   "OK",   200 , NULL , NULL}
};

AT_Command_t CAT_Init_Cmd[AT_COMMAND_ARRAY_SIZE] ={
    {"AT\r\n",      "OK", 1000 , NULL ,     Device_RST_Hard},
    {"AT+CGSN\r\n", "OK", 1000 , Get_IMEI , Device_RST_Soft},
    {"AT+CCID\r\n", "OK", 1000 , Get_CCID , Device_RST_Soft}

};

AT_URC_t AT_URC_Msg[AT_COMMAND_ARRAY_SIZE] = {
    {"WIFI GOT IP",             NULL},
    {"WIFI DISCONNECT",         NULL},
    {"CLOSED",                  Device_RST_Soft},
    {"SEND OK",                 NULL},
    {"ERROR",                   ERROR_CallBack},
    {"NO Service",              Device_RST_Soft}
};


void AT_Init(void *params)
{
    static uint8_t *step = &AT_Device.init_step;
    AT_Device.status = AT_DISCONNECT;
    static AT_Command_t *AT_cmd = AT_Init_Cmd;
    while(1)
    {
       if (AT_SendCmd(AT_cmd[*step].cmd, AT_cmd[*step].response, AT_cmd[*step].timeout) != 0)
       {
            if(AT_cmd[*step].ack_err_response != NULL)
            {
                AT_cmd[*step].ack_err_response();
            }
       }
       else
       {
            if(AT_cmd[*step].ack_right_response != NULL)
            {
                AT_cmd[*step].ack_right_response();
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
//                result = rt_thread_init(&AT_Init_Thread, "AT_Init", AT_Init, RT_NULL, &AT_Init_ThreadStack[0], AT_THREAD_STACK_SIZE, 10, 10);
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
                    if(AT_SendCmd(ESP_Command[0].cmd, ESP_Command[0].response, ESP_Command[0].timeout) == 0)
                    {
                        rt_kprintf("AT_SendCmd success\n");
                    }
                    else
                    {
                        rt_kprintf("AT_SendCmd failed\n");
                    }
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
            if(strstr((char *)&AT_Device.at_uart_device->rx_buffer[AT_DATA_START_BIT], AT_URC_Msg[i].urc_msg) != NULL)
            {
                if(AT_URC_Msg[i].response != NULL)
                {
                    AT_URC_Msg[i].response();
                }
            }
		 }        
		rt_thread_mdelay(10);
	}
}


void AT_START(void)
{
    rt_err_t result;
    at_device_register(AT_PORT , AT_BAUD);
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

void at_list(void)
{
	uint8_t i = 0;
	rt_kprintf("********************AT CMD LIST********************\n");
	for(i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
	{
			if(ESP_Command[i].cmd != NULL)
			{
					rt_kprintf("AT_CMD[%d]: %s\n", i, ESP_Command[i].cmd);
			}
	}

	rt_kprintf("********************CAT CMD LIST********************\n");
	for(i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
	{
			if(CAT_Command[i].cmd != NULL)
			{
					rt_kprintf("CAT_CMD[%d]: %s\n", i, CAT_Command[i].cmd);
			}
	}

	rt_kprintf("********************AT INIT LIST********************\n");
	for(i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
	{
			if(AT_Init_Cmd[i].cmd != NULL)
			{
					rt_kprintf("AT_INIT_CMD[%d]: %s\n", i, AT_Init_Cmd[i].cmd);
			}
	}

	rt_kprintf("********************AT URC LIST********************\n");
	for(i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
	{
			if(AT_URC_Msg[i].urc_msg != NULL)
			{
					rt_kprintf("AT_URC[%d]: %s\n", i, AT_URC_Msg[i].urc_msg);
			}
	}
}

MSH_CMD_EXPORT(at_list, list at cmd);
