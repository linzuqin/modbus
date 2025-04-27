#include "AT_Function.h"
#include "list_status.h"


extern AT_URC_t AT_URC_Msg[AT_COMMAND_ARRAY_SIZE];
extern AT_Command_t AT_Init_Cmd[AT_COMMAND_ARRAY_SIZE];
extern AT_Command_t AT_Cmd[AT_COMMAND_ARRAY_SIZE];
extern AT_Device_t AT_Device;
static struct rt_timer timer1;

uint8_t AT_SendCmd( AT_Device_t *at_device , const char *cmd , const char *response, uint16_t timeout)
{
    if (cmd == NULL)
    {
        return 1; // Successfully added command
    }

    uint8_t i;
    uint8_t timeout_count = 5;

	for (i = 0; cmd[i] != '\0'; i ++)
	{
		USART_SendData(AT_PORT, cmd[i]);
		while (USART_GetFlagStatus(AT_PORT, USART_FLAG_TC) == RESET);
	}
    
	//LOG_I("->: %s", cmd);

	if(response != NULL)
	{
		//LOG_I("AT_Device.rx_buf: %s AT_RX_SIZE:%d", AT_Device.at_uart_device->rx_buffer , AT_Device.at_uart_device->rx_size);

		while(strstr((char *)&(at_device->rx_buf[AT_DATA_START_BIT]), response) == NULL)
		{
				if (timeout_count-- == 0)
				{
								return 2;
				}
				rt_thread_mdelay(timeout);
		}
		//LOG_I("<-: %s", response);
	}
    return 1;
}

uint8_t AT_Cmd_Regsiter(AT_Command_t *cmd_array , const char *response, uint16_t timeout, void (*ack_right_response)(void), void (*ack_err_response)(void) , const char *cmd, ...)
{
    if (cmd == NULL || response == NULL)
    {
        return 1;
    }
    char cmd_buf[256];
    va_list ap;
    
    va_start(ap, cmd);
    vsnprintf((char *)cmd_buf, sizeof(cmd_buf), cmd, ap);							// Format the command string
	va_end(ap);

    LOG_I("->: %s", cmd_buf);

    AT_Command_t AT_Command = {0};
    AT_Command.cmd = (char *)malloc(strlen(cmd_buf) + 1);
    if (AT_Command.cmd == NULL)
    {
        LOG_I("Memory allocation failed for AT_Command.cmd\n");
        return 0;
    }
    AT_Command.ack_err_response = ack_err_response;
    AT_Command.ack_right_response = ack_right_response;
    AT_Command.timeout = timeout;
    AT_Command.response = response;
    
    // Maintain a static index to track the next available slot
    static int next_available_slot = 0;

    for (int i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
    {
        int index = (next_available_slot + i) % AT_COMMAND_ARRAY_SIZE;
        if (cmd_array[index].cmd == NULL)
        {
            cmd_array[index] = AT_Command;
            strcpy((char *)cmd_array[index].cmd, cmd_buf); // Copy the command string into the allocated memory
            next_available_slot = (index + 1) % AT_COMMAND_ARRAY_SIZE;
            LOG_I("AT Cmd Add success: %s\n", cmd_array[index].cmd);
            return 1;
        }
    }
    LOG_I("AT Cmd Add fail: No available slots in AT_Command_array\n");
    free((char *)AT_Command.cmd); // Free allocated memory if adding fails

    return 0;
}

static void AT_RST_GPIO_Init(void)
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

void at_device_register(USART_TypeDef *USARTx , uint32_t bound , AT_Device_t *at_device , uint8_t *rx_buffer)
{
		at_device->rx_buf = rx_buffer;
    AT_RST_GPIO_Init();
    My_UART_Init(USARTx , bound);
}

void Device_RST_Soft(AT_Device_t *at_device)
{
    at_device->status = AT_DISCONNECT;
    at_device->init_step = 0;
    LOG_I("Device soft reset initiated\n");
}

void Device_RST_Hard(void)
{
    GPIO_ResetBits(AT_RST_PORT, AT_RST_PIN);
    rt_thread_mdelay(3000);
    GPIO_SetBits(AT_RST_PORT, AT_RST_PIN);
}

list_status_t* at_current = NULL;
list_status_t* at_status1 = NULL;
list_status_t* at_status2 = NULL;
list_status_t* at_status3 = NULL;


void AT_IDLE_Handle(void)
{
	LOG_I("Running AT_IDLE state\n");
}

void AT_UP_Handle(void)
{
	LOG_I("Running AT UP state\n");
	
}

void AT_UP_Exit(void)
{
	LOG_I("Exiting AT_UP state\n");
	list_status_remove(at_status2);
}

void timeout1(void*params)
{
	if(at_status1->next != at_status2)
	{
		list_status_add(&at_status1 , at_status2);
	}
}

void at_list_init(void)
{
    at_status1 = list_status_create("AT_IDLE" , NULL , NULL , AT_IDLE_Handle);
    at_status2 = list_status_create("AT_UP" , NULL , AT_UP_Exit , AT_UP_Handle);
    //at_status3 = list_status_create("AT_IDLE2" , AT_IDLE_Enter , AT_IDLE_Exit , AT_IDLE_Handle);
    rt_timer_init(&timer1, "timer1",  /* ��ʱ�������� timer1 */
        timeout1, /* ��ʱʱ�ص��Ĵ������� */
        RT_NULL, /* ��ʱ��������ڲ��� */
        5000, /* ��ʱ���ȣ��� OS Tick Ϊ��λ���� 10 �� OS Tick */
        RT_TIMER_FLAG_PERIODIC); /* �����Զ�ʱ�� */
   rt_timer_start(&timer1);
}

void at_list_poll(void)
{
   list_poll(at_status1 , &at_current);
}

void at_cmd_list(void)
{
	uint8_t i = 0;
	rt_kprintf("********************AT CMD LIST********************\n");
	for(i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
	{
			if(AT_Cmd[i].cmd != NULL)
			{
					rt_kprintf("cmd[%d]: %s\n", i, AT_Cmd[i].cmd);
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
MSH_CMD_EXPORT(at_cmd_list, list at cmd);


void list_printf(void)
{
    list_status_t *current = at_status1;
    while (current != NULL)
    {
        rt_kprintf("Status: %s\n", current->name);
        current = current->next;
    }
}
MSH_CMD_EXPORT(list_printf, list at status);
