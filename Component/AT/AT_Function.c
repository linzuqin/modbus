#include "AT_Function.h"


extern AT_Device_t AT_Device;


uint8_t AT_SendCmd(const char *cmd , const char *response, uint16_t timeout)
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
		while(AT_Device.at_uart_device->rx_flag == 0)
		{
				if(timeout_count++ > 5)
				{
						return 2;
				}
				rt_thread_mdelay(100);
		}

		AT_Device.at_uart_device->rx_flag = 0;
		//LOG_I("AT_Device.rx_buf: %s AT_RX_SIZE:%d", AT_Device.at_uart_device->rx_buffer , AT_Device.at_uart_device->rx_size);

		while(strstr((char *)&AT_Device.at_uart_device->rx_buffer[AT_DATA_START_BIT], response) == NULL)
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
            strcpy(cmd_array[index].cmd, cmd_buf); // Copy the command string into the allocated memory
            next_available_slot = (index + 1) % AT_COMMAND_ARRAY_SIZE;
            LOG_I("AT Cmd Add success: %s\n", cmd_array[index].cmd);
            return 1;
        }
    }
    LOG_I("AT Cmd Add fail: No available slots in AT_Command_array\n");
    free(AT_Command.cmd); // Free allocated memory if adding fails

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

void at_device_register(USART_TypeDef *USARTx , uint32_t bound)
{
    if (USARTx == USART1)
    {
        AT_Device.at_uart_device = &uart1_device;
    }
    else if (USARTx == USART2)
    {
        AT_Device.at_uart_device = &uart2_device;
    }
    else if (USARTx == USART3)
    {
        AT_Device.at_uart_device = &uart3_device;
    }
    else if (USARTx == UART4)
    {
        AT_Device.at_uart_device = &uart4_device;
    }

    AT_RST_GPIO_Init();
    My_UART_Init(USARTx , bound);
}


void Device_RST_Soft(void)
{
    AT_Device.status = AT_DISCONNECT;
    AT_Device.init_step = 0;
    LOG_I("Device soft reset initiated\n");
}

void Device_RST_Hard(void)
{
    GPIO_ResetBits(AT_RST_PORT, AT_RST_PIN);
    rt_thread_mdelay(3000);
    GPIO_SetBits(AT_RST_PORT, AT_RST_PIN);
}

void ERROR_CallBack(void)
{
    static uint8_t error_count = 0;
    error_count ++;
    if(error_count > 5)
    {
        error_count = 0;
        if(AT_Device.status == AT_DISCONNECT)
        {
            Device_RST_Soft();
            LOG_I("Device soft reset initiated\n");
        }
        else
        {
            Device_RST_Hard();
            LOG_I("Device hard reset initiated\n");
        }
    }
    else
    {
        LOG_I("ack error :%d\n",error_count);
    }
}

void Get_IMEI(void)
{
    memcpy(AT_Device.IMEI, &AT_Device.at_uart_device->rx_buffer[AT_DATA_START_BIT], 15);
    LOG_I("IMEI: %s\n", AT_Device.IMEI);
}

void Get_CCID(void)
{
    memcpy(AT_Device.ICCID, &AT_Device.at_uart_device->rx_buffer[AT_DATA_START_BIT], 20);
    LOG_I("IMEI: %s\n", AT_Device.IMEI);
}