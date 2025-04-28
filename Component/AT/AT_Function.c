#include "AT_Function.h"
#include "list_status.h"
/*lora  rst:pa8 默认高电平   wake:pB15*/
/*AT DEVICE CMD*/
AT_Command_t CAT_Init_Cmd[AT_COMMAND_ARRAY_SIZE] ={
    {"AT\r\n",          "OK",   100 , NULL , NULL},
    {"AT+RESET\r\n",    "NITZ", 2000 , NULL , NULL},
    // {"AT+MCONFIG=\"qihoushi\",\"c1Lost570Z\",\"version=2018-10-31&res=products%2Fc1Lost570Z%2Fdevices%2Fqihoushi&et=1924876800&method=md5&sign=hO9c3NIdBtNE1mWAPBsZiQ%3D%3D\"\r\n", "OK",   100 , NULL , NULL},
    // {"AT+MIPSTART=\"mqtts.heclouds.com\",1883\r\n","CONNECT OK",   100 , NULL , NULL},
    // {"AT+MCONNECT=1,120\r\n","CONNACK OK",   100 , NULL , NULL},
    // {"AT+MSUB=\"$sys/c1Lost570Z/qihoushi/thing/property/set\",0\r\n","OK",   100 , NULL , NULL}
};

AT_Command_t AT_Init_Cmd[AT_COMMAND_ARRAY_SIZE] ={
    {"AT\r\n", "OK", 1000 , NULL , NULL}
    //{"AT+RST\r\n", "OK", 2000 , NULL , NULL}
};

AT_URC_t AT_URC_Msg[AT_COMMAND_ARRAY_SIZE] = {
    {"WIFI GOT IP",             NULL},
    {"WIFI DISCONNECT",         NULL},
    {"CLOSED",                  Device_RST_Soft},
    {"SEND OK",                 NULL},
    {"ERROR",                   NULL},
    {"SIMDETEC: 1,NOS",         Device_RST_Soft}
};

AT_Command_t CAT_Run_Cmd[AT_COMMAND_ARRAY_SIZE] = {
};

uint8_t AT_SendCmd( AT_Device_t *at_device , const char *cmd , const char *response , uint16_t timeout) 
{
    if (cmd == NULL)
    {
        return 0; // Successfully added command
    }

    uint8_t i;
    uint8_t timeout_count = 5;

	for (i = 0; cmd[i] != '\0'; i ++)
	{
		USART_SendData(at_device->PORT, cmd[i]);	// Send the command byte by byte
		while (USART_GetFlagStatus(at_device->PORT, USART_FLAG_TC) == RESET);
	}
    
	//LOG_I("->: %s", cmd);

	if(response != NULL)
	{
		//LOG_I("AT_Device.rx_buf: %s AT_RX_SIZE:%d", AT_Device.at_uart_device->rx_buffer , AT_Device.at_uart_device->rx_size);
		
		while(strstr((char *)&(at_device->rx_buf[0]), response) == NULL)
		{
			if (timeout_count-- == 0)
			{
					rt_memset(at_device->rx_buf , 0 , sizeof(at_device->rx_buf));
					return 2;
			}
			rt_thread_mdelay(timeout);
		}
		//LOG_I("<-: %s", response);
	}
		rt_memset(at_device->rx_buf , 0 , sizeof(at_device->rx_buf));
    return 0;
}

uint8_t AT_Cmd_Regsiter(AT_Command_t *at_cmd_array , const char *response, uint16_t timeout, void (*ack_right_response)(void), void (*ack_err_response)(void) , const char *cmd, ...)
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
    int next_available_slot = 0;
		for(uint8_t i = 0;i < AT_COMMAND_ARRAY_SIZE ; i ++)
		{
			if(at_cmd_array[i].cmd == NULL)
			{
				next_available_slot = i;
				break;
			}
		}
		
    for (int i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
    {
        int index = (next_available_slot + i) % AT_COMMAND_ARRAY_SIZE;
        if (at_cmd_array[index].cmd == NULL)
        {
            at_cmd_array[index] = AT_Command;
            strcpy((char *)at_cmd_array[index].cmd, cmd_buf); // Copy the command string into the allocated memory
            next_available_slot = (index + 1) % AT_COMMAND_ARRAY_SIZE;
            LOG_I("AT Cmd Add success: %s\n", at_cmd_array[index].cmd);
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

void at_device_register(AT_Device_t *at_device  , USART_TypeDef *USARTx , uint32_t bound , uint8_t **rx_buffer , uint8_t *rx_flag , AT_Command_t *init_cmd , AT_Command_t *run_cmd)
{
    at_device->PORT = USARTx;
    at_device->Bound = bound;
    My_UART_Init(at_device->PORT , at_device->Bound);

    at_device->init_cmd = init_cmd;
    at_device->run_cmd = run_cmd;

    at_device->rx_buf = *rx_buffer;
	at_device->rx_flag = rx_flag;
    at_device->status = AT_DISCONNECT; // Initialize status
    at_device->init_step = 0;         // Initialize init_step
    AT_RST_GPIO_Init();
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
