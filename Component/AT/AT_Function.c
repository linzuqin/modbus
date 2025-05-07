#include "AT_Function.h"
/*AT DEVICE CMD*/
AT_Device_t AT_Device = {
    .status = AT_NO_REGISTER, // Initialize status
    .init_step = 0,         // Initialize init_step
    .rx_buf = NULL,         // Initialize rx_buf to NULL
    .rx_flag = NULL,        // Initialize rx_flag to NULL
    .PORT = NULL,           // Initialize PORT to NULL
    .Bound = 0,             // Initialize Bound to 0
    .CMD_TABLE = NULL,      // Initialize CMD_TABLE to NULL
    .URC_TABLE = NULL       // Initialize URC_TABLE to NULL
};

AT_CMD_t AT_Cmd_table[AT_COMMAND_ARRAY_SIZE] ={
    {"AT\r\n",           "OK",   500 ,  NULL},
    {"AT+CWMODE=1\r\n",  "OK",   500 , NULL},
    {"AT+RST\r\n",     	 "ready",   3000 , NULL},
    {"AT+CWDHCP=1,1\r\n","OK",   500 , NULL},
};

AT_URC_t AT_URC_table[AT_COMMAND_ARRAY_SIZE] = {
    {"WIFI GOT IP",             NULL},
    {"WIFI DISCONNECT",         NULL},
    {"CLOSED",                  NULL},
    {"SEND OK",                 NULL},
    {"ERROR",                   NULL},
    {"SIMDETEC: 1,NOS",         NULL}
};

uint8_t AT_SendCmd( AT_Device_t *at_device , const char *cmd , const char *response , uint16_t timeout) 
{
    if (cmd == NULL)
    {
        return 0; // Successfully added command
    }

    uint16_t i;
    uint8_t timeout_count = 5;
    uint16_t len = strlen(cmd);
		LOG_I("len : %d" , len);
	for (i = 0; i< len; i ++)
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
		LOG_I("<-: %s", response);
	}
		rt_memset(at_device->rx_buf , 0 , sizeof(at_device->rx_buf));
    return 0;
}

/*发布消息指令 根据实际AT设备指令调整*/
uint8_t mqtt_pub(AT_Device_t *at_device , char *data , uint16_t len , char *topic)
{
	//AT_SendCmd("\r\nAT+MPUBEX=\"%s\",0,0,%d\r\n",topic,1000);
	char cmd[256];
	sprintf(cmd , "AT+MQTTPUBRAW=0,\"%s\",%d,0,0\r\n" , topic , len);
	if(AT_SendCmd(at_device,cmd , ">" ,1000) != 0)return 1;
	return AT_SendCmd(at_device,data,"OK",1000);
}

uint8_t AT_Cmd_Regsiter(AT_Device_t *at_device , const char *response, uint16_t timeout, void (*callback_response)(void), int insert_count , const char *cmd, ...)
{
    if (cmd == NULL || response == NULL)
    {
        return 1;
    }

    if(at_device->CMD_TABLE == NULL)
    {
        return 2; // Error: CMD_TABLE is not initialized
    }

    char cmd_buf[256];
    va_list ap;
    
    va_start(ap, cmd);
    vsnprintf((char *)cmd_buf, sizeof(cmd_buf), cmd, ap);							// Format the command string
	va_end(ap);

    //LOG_I("->: %s", cmd_buf);

    AT_CMD_t AT_Command = {0};
    AT_Command.cmd = (char *)malloc(strlen(cmd_buf) + 1);
    if (AT_Command.cmd == NULL)
    {
        //LOG_I("Memory allocation failed for AT_Command.cmd\n");
        return 3;
    }
    AT_Command.callback_response = callback_response;
    AT_Command.timeout = timeout;
    AT_Command.response = response;
    
    // Maintain a static index to track the next available slot
    if(insert_count < 0 || insert_count >= AT_COMMAND_ARRAY_SIZE)
    {
        int next_available_slot = 0;
            for(uint8_t i = 0;i < AT_COMMAND_ARRAY_SIZE ; i ++)
            {
                if(at_device->CMD_TABLE[i].cmd == NULL)
                {
                    next_available_slot = i;
                    break;
                }
            }
            
        for (int i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
        {
            int index = (next_available_slot + i) % AT_COMMAND_ARRAY_SIZE;
            if (at_device->CMD_TABLE[index].cmd == NULL)
            {
                strcpy((char *)AT_Command.cmd, cmd_buf); // Copy the command string into the allocated memory
                at_device->CMD_TABLE[index] = AT_Command;
                next_available_slot = (index + 1) % AT_COMMAND_ARRAY_SIZE;
                LOG_I("AT Cmd Add success: %s\n", at_device->CMD_TABLE[index].cmd);
                return 0;
            }
        }
    }else{
        at_device->CMD_TABLE[insert_count].cmd = (char *)malloc(strlen(cmd_buf) + 1);
        if (at_device->CMD_TABLE[insert_count].cmd != NULL)
        {
            strcpy((char *)at_device->CMD_TABLE[insert_count].cmd, cmd_buf);
            at_device->CMD_TABLE[insert_count].response = response;
            at_device->CMD_TABLE[insert_count].timeout = timeout;
            at_device->CMD_TABLE[insert_count].callback_response = callback_response;
        }
        else
        {
            LOG_E("Memory allocation failed for CMD_TABLE[insert_count].cmd");
        }
    }
    LOG_I("AT Cmd Add fail: No available slots in AT_Command_array\n");
    free((char *)AT_Command.cmd); // Free allocated memory if adding fails

    return 5;
}

void at_device_register(AT_Device_t *at_device  , uart_device_t *uart_device , AT_CMD_t *cmd_table , AT_URC_t *urc_table)
{
    at_device->PORT = uart_device->port;
    at_device->Bound = uart_device->baudrate;
    //My_UART_Init(at_device);

    at_device->CMD_TABLE = cmd_table;
    at_device->URC_TABLE = urc_table;

    at_device->rx_buf = uart_device->rx_buffer;
	at_device->rx_flag = &uart_device->rx_flag;
    at_device->status = AT_DISCONNECT; // Initialize status
    at_device->init_step = 0;         // Initialize init_step
}

void user_cmd_regsiter(void)
{
    AT_Cmd_Regsiter(&AT_Device , "WIFI CONNECTED" , 1000 , NULL , 1 , "AT+CWJAP=\"%s\",\"%s\"\r\n",DEFAULT_WIFI_SSID , DEFAULT_WIFI_PWD);
    AT_Cmd_Regsiter(&AT_Device , "OK" , 1000 , NULL , 1 , "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",DEVICE_NAME , PRODUCT_ID , TOKEN);
    AT_Cmd_Regsiter(&AT_Device , "OK" , 1000 , NULL , 1 , "AT+MQTTCONN=0,\"%s\",%d,1\r\n",IP_ADDRESS , PORT_NUMBER);
    AT_Cmd_Regsiter(&AT_Device , "OK" , 1000 , NULL , 1 , "AT+MQTTSUB=0,\"%s\",1\r\n",SET_TOPIC_ALL);
}

static void AT_IDLE_TASK(AT_Device_t *at_device)
{
    uint16_t urc_size = sizeof(AT_URC_table)/sizeof(AT_URC_table[0]);
    uint16_t msg_size = 0;
    char *urc_msg = NULL;
    void (*callback)(void) = NULL;
    
    if((*at_device->rx_flag) == 1)
    {
        (*at_device->rx_flag) = 0;
        for(uint16_t i = 0; i < urc_size; i++)
        {
            if(at_device->URC_TABLE[i].urc_msg != NULL)
            {
                urc_msg =  strstr((char *)&(at_device->rx_buf[0]), at_device->URC_TABLE[i].urc_msg);
                if(urc_msg != NULL)
                {
                    msg_size = sizeof(at_device->URC_TABLE[i].urc_msg);
                    callback = at_device->URC_TABLE[i].callback;
                    break;
                }
            }
        } 
        
        if(msg_size != 0)                     //若为urc数据则正常执行回调函数
        {
            if(callback != NULL)
            {
                callback();
            }
            //LOG_I("URC: %s", at_device->URC_TABLE[i].urc_msg);
            rt_memset(urc_msg , 0 , msg_size); // Clear the URC message from the buffer
        }
        else                                    //若为其他数据 转为AT_PARSE状态
        {
            at_device->status = AT_PARSE; // Set status to AT_PARSE if no URC message found
        }
    }
}

static void AT_PARSE_TASK(AT_Device_t *at_device)
{

}

void AT_poll(AT_Device_t *at_device)
{
    switch(at_device->status)
    {
        case AT_NO_REGISTER:
            My_UART_Init(&AT_DEFAULT_UART_DEVICE);
            at_device_register(at_device , &AT_DEFAULT_UART_DEVICE , AT_Cmd_table , AT_URC_table);
            at_device->status = AT_REGISTERED;
            //LOG_I("AT Device not registered\n");
            user_cmd_regsiter();
            break;

        case AT_REGISTERED:
            // AT_SendCmd(at_device , "AT\r\n");
            // AT_SendCmd(at_device , AT_Cmd_table[1].cmd);
            // AT_SendCmd(at_device , AT_Cmd_table[2].cmd);
            // AT_SendCmd(at_device , AT_Cmd_table[3].cmd);
            // AT_SendCmd(at_device , AT_Cmd_table[4].cmd);
            // AT_SendCmd(at_device , AT_Cmd_table[5].cmd);
            // AT_SendCmd(at_device , AT_Cmd_table[6].cmd);
            // AT_SendCmd(at_device , AT_Cmd_table[7].cmd);
            // AT_SendCmd(at_device , AT_Cmd_table[8].cmd);

            //LOG_I("AT Device registered\n");
            break;

        case AT_DISCONNECT:
            //LOG_I("AT Device disconnected\n");
            break;

        case AT_CONNECT:
            //LOG_I("AT Device connected\n");
            break;

        case AT_IDEL:
            //LOG_I("AT Device idle\n");
            AT_IDLE_TASK(at_device);
            break;

        case AT_PARSE:
            //LOG_I("AT Device parsing\n");
            AT_PARSE_TASK(at_device);
            break;

        case AT_UPDATA:
            //LOG_I("AT Device updating\n");
            break;
        default:break;
    }
}

void AT_Thread_Entry(void *parameter)
{

    while (1)
    {
        // Check if the device is registered
        AT_poll(&AT_Device);

        rt_thread_mdelay(1000); // Delay for 1 second before next iteration
    }
}
struct rt_thread at_thread;
static uint8_t at_thread_stack[4096];
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
        return 0; // Initialization successful
    }
    else
    {
        LOG_E("Failed to initialize AT thread");
        return -1; // Initialization failed
    }
}

INIT_APP_EXPORT(AT_Thread_Init);
