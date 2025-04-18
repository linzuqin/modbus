#include "AT_Device.h"
#include "rtthread.h"
#include "string.h"
#include <stdint.h>
#include "log.h"
#include <stdio.h>

#define AT_PORT  USART1
#define AT_BAUD  115200
#define AT_SIZE  256
uint8_t *AT_RxData;

#define AT_THREAD_STACK_SIZE 1024
static struct rt_thread AT_Thread;
static uint8_t AT_ThreadStack[AT_THREAD_STACK_SIZE];

static struct rt_thread AT_Init_Thread;
static uint8_t AT_Init_ThreadStack[AT_THREAD_STACK_SIZE];

static struct rt_thread AT_URC_Thread;
static uint8_t AT_URC_ThreadStack[AT_THREAD_STACK_SIZE];

static struct rt_mailbox mb;
static uint8_t mb_buffer[32];

AT_Device_t AT_Device;
/*ESP DEVICE CMD*/
AT_Command_t ESP_Command[] = {
    {"AT\r\n", "OK", 100 , NULL , NULL},
    {"AT+GMR\r\n", "OK", 100 , NULL , NULL},
    {"AT+RST\r\n", "OK", 200 , NULL , NULL},
    {"AT+CWMODE?\r\n", "OK", 100 , NULL , NULL},
    {"AT+CWJAP?\r\n", "OK", 200 , NULL , NULL},
    {"AT+CIPSTART\r\n", "OK", 500 , NULL , NULL},
    {"AT+CIPSEND\r\n", ">", 500 , NULL , NULL},
    {"AT+CIPCLOSE\r\n", "OK", 200 , NULL , NULL},
    {"AT+CWQAP\r\n`", "OK", 200 , NULL , NULL}
};

AT_Command_t AT_Init_Cmd[] ={
    {"AT\r\n", "OK", 1000 , NULL , NULL}
    //{"AT+RST\r\n", "OK", 2000 , NULL , NULL}
};


AT_Command_t CAT_Command[] = {
    {"AT\r\n", "OK", 100 , NULL , NULL},
    {"AT+GMR\r\n", "OK", 100 , NULL , NULL},
    {"AT+RST\r\n", "OK", 200 , NULL , NULL},
    {"AT+CWMODE?\r\n", "OK", 100 , NULL , NULL},
    {"AT+CWJAP?\r\n", "OK", 200 , NULL , NULL},
    {"AT+CIPSTART\r\n", "OK", 500 , NULL , NULL},
    {"AT+CIPSEND\r\n", ">", 500 , NULL , NULL},
    {"AT+CIPCLOSE\r\n", "OK", 200 , NULL , NULL},
    {"AT+CWQAP\r\n`", "OK", 200 , NULL , NULL}
};
AT_Command_t CAT_Init_Cmd[] ={
    {"AT\r\n", "OK", 1000 , NULL , Device_RST_Hard},
    {"AT+CGSN\r\n", "OK", 1000 , Get_IMEI , Device_RST_Soft},
    {"AT+CCID\r\n", "OK", 1000 , Get_CCID , Device_RST_Soft}

};

AT_URC_t AT_URC_Msg[] = {
    {"WIFI GOT IP", NULL},
    {"WIFI DISCONNECT", NULL},
    {"CLOSED", Device_RST_Soft},
    {"SEND OK", NULL},
    {"ERROR", ERROR_CallBack},
    {"NO Service",Device_RST_Soft}
};

void AT_RST_GPIO_Init(void)
{
    // Enable GPIOC clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // Configure PC5 as output push-pull
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(AT_RST_PORT, &GPIO_InitStructure);

    // Set PC5 high
    GPIO_SetBits(AT_RST_PORT, AT_RST_PIN);
}

uint8_t AT_SendCmd(char *cmd , char *response, uint16_t timeout)
{
    if (cmd == NULL)
    {
        return 1;
    }

    uint8_t i;
    uint8_t timeout_count = 5;
    rt_memset(&AT_RxData[2], 0, AT_SIZE - 2);

	for (i = 0; cmd[i] != '\0'; i ++)
	{
		USART_SendData(AT_PORT, cmd[i]);
		while (USART_GetFlagStatus(AT_PORT, USART_FLAG_TC) == RESET);
	}
	LOG_I("->: %s", cmd);

	if(response != NULL)
	{
        AT_RxData[AT_SIZE - 1] = '\0'; // Ensure null-termination
        while(strstr((char *)AT_RxData, response) == NULL)
        {
            if (timeout_count-- == 0)
            {
                    return 2;
            }
            rt_thread_mdelay(timeout);
        }
        LOG_I("<-: %s", response);
	}
	return 0;
}

void AT_Init(void *params)
{
    uint8_t *step = &AT_Device.init_step;
    AT_Device.status = AT_DISCONNECT;
    while(1)
    {
       if (AT_SendCmd(AT_Init_Cmd[*step].cmd, AT_Init_Cmd[*step].response, AT_Init_Cmd[*step].timeout) != 0)
       {
            if(AT_Init_Cmd[*step].ack_err_response != NULL)
            {
                AT_Init_Cmd[*step].ack_err_response();
            }
       }
       else
       {
            if(AT_Init_Cmd[*step].ack_right_response != NULL)
            {
                AT_Init_Cmd[*step].ack_right_response();
            }
            (*step) ++;

            if (*step >= sizeof(AT_Init_Cmd)/sizeof(AT_Command_t))
            {
                if(AT_Thread.stat != RT_THREAD_RUNNING)
                {
                    rt_thread_startup(&AT_Thread);
                }
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

void AT_START(void)
{
    rt_err_t result;
    //UART1_Init(AT_BAUD);
    AT_RST_GPIO_Init();
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