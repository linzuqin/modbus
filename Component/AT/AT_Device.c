#include "AT_Device.h"
#include "rtthread.h"
#include "string.h"
#include <stdint.h>

#define AT_PORT  USART1
#define AT_BAUD  115200
#define AT_SIZE  256
static uint8_t AT_RxData[AT_SIZE];

#define AT_THREAD_STACK_SIZE 512
static struct rt_thread AT_Thread;
static uint8_t AT_ThreadStack[AT_THREAD_STACK_SIZE];

static struct rt_thread AT_Init_Thread;
static uint8_t AT_Init_ThreadStack[AT_THREAD_STACK_SIZE];

static struct rt_thread AT_URC_Thread;
static uint8_t AT_URC_ThreadStack[AT_THREAD_STACK_SIZE];

static struct rt_mailbox mb;
static uint8_t mb_buffer[32];

AT_Device_t AT_Device;

static AT_Command_t AT_Command[] = {
    {"AT", "OK", 1000},
    {"AT+GMR", "OK", 1000},
    {"AT+RST", "OK", 2000},
    {"AT+CWMODE?", "OK", 1000},
    {"AT+CWJAP?", "OK", 2000},
    {"AT+CIPSTART", "OK", 5000},
    {"AT+CIPSEND", ">", 5000},
    {"AT+CIPCLOSE", "OK", 2000},
    {"AT+CWQAP", "OK", 2000}
};

static  AT_Command_t AT_Init_Cmd[] ={
    {"AT", "OK", 1000},
    {"AT+RST", "OK", 2000}
};

static AT_URC_t AT_URC_Msg[] = {
    {"WIFI GOT IP", NULL},
    {"WIFI DISCONNECT", NULL},
    {"CLOSED", NULL},
    {"SEND OK", NULL},
    {"ERROR", NULL}
};

uint8_t AT_SendCmd(char *cmd , char *response, uint16_t timeout)
{
	uint8_t i;
    uint8_t timeout_count = 5;
	for (i = 0; cmd[i] != '\0'; i ++)
	{
		USART_SendData(AT_PORT, cmd[i]);
	    while (USART_GetFlagStatus(AT_PORT, USART_FLAG_TXE) == RESET);
	}

    USART_SendData(AT_PORT, '\r');
    USART_SendData(AT_PORT, '\n');

    while (USART_GetFlagStatus(AT_PORT, USART_FLAG_TXE) == RESET);

    if(response != NULL)
    {
        while(strstr((char *)AT_RxData, response) == NULL)
        {
            if (timeout_count-- == 0)
            {
                return 0;
            }
            rt_thread_mdelay(timeout);
        }
    }
    return 1;
}


void AT_TASK(void *params)
{
    while(1)
    {
        if(AT_Device.status == AT_DISCONNECT)
        {
            if(AT_Init_Thread.stat == RT_THREAD_SUSPEND)
            {
                rt_thread_resume(&AT_Init_Thread);
                rt_kprintf("AT_Init_Thread resume\n");
            }
        }
        else {
            switch(AT_Device.status)
            {
                case AT_CONNECT:AT_Device.status = AT_IDEL;break;
                case AT_IDEL:
                {

                }
                break;
                case AT_SEND:
                    if(AT_SendCmd(AT_Command[0].cmd, AT_Command[0].response, AT_Command[0].timeout) == 0)
                    {
                        rt_kprintf("AT_SendCmd failed\n");
                    }
                    else
                    {
                        rt_kprintf("AT_SendCmd success\n");
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
        if(AT_Device.status == AT_CONNECT)
        {
            for(uint8_t i = 0; i < sizeof(AT_URC_Msg)/sizeof(AT_URC_t); i++)
            {
                if(strstr((char *)AT_RxData, AT_URC_Msg[i].urc_msg) != NULL)
                {
                    if(AT_URC_Msg[i].response != NULL)
                    {
                        AT_URC_Msg[i].response();
                    }
                    rt_memset(AT_RxData, 0, sizeof(AT_RxData));
                }
            }
        }
        
        rt_thread_mdelay(10);
    }
}

void AT_Init(void *params)
{
    uint8_t step = 0;
    AT_Device.status = AT_DISCONNECT;
    while(1)
    {
        if (AT_SendCmd(AT_Init_Cmd[step].cmd, AT_Init_Cmd[step].response, AT_Init_Cmd[step].timeout) == 0)
        {
            step = 0;
        }
        else
        {
            step++;
            if (step >= sizeof(AT_Init_Cmd)/sizeof(AT_Command_t))
            {
                if(rt_thread_suspend(&AT_Init_Thread) == RT_EOK)
                {
                    step = 0;
                    AT_Device.status = AT_CONNECT;
                    rt_schedule();
                    rt_thread_startup(&AT_Thread);
                }
                else
                {
                    rt_kprintf("AT_Init_Thread suspend failed\n");
                }


                break;
            }
        }
        rt_thread_mdelay(1000);
    }
}


void AT_START(void)
{
    rt_err_t result;
    result = rt_thread_init(&AT_Init_Thread, "AT_Init", AT_Init, RT_NULL, &AT_Init_ThreadStack[0], AT_THREAD_STACK_SIZE, 10, 10);
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
    result = rt_thread_init(&AT_URC_Thread, "AT_URC", AT_Init, RT_NULL, &AT_URC_ThreadStack[0], AT_THREAD_STACK_SIZE, 10, 10);
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

}