#include "AT_Function.h"

/*指令的回调函数*/
static void MQTTDISCONNECTED_CallBack(void *device)
{
	AT_Device_t *at_device = device;
	at_device->init_step = 0;
	at_device->status = AT_INIT;
}

/*指令集*/
AT_CMD_t AT_Cmd_table[AT_COMMAND_ARRAY_SIZE] = {
    {"AT\r\n", "OK", 1000, NULL},
    {"ATE1\r\n", "OK", 1000, NULL},

    {"AT+CWMODE=1\r\n", "OK", 1000, NULL},
    {"AT+RST\r\n", "OK", 5000, NULL},
    {"AT+CWDHCP=1,1\r\n", "OK", 1000, NULL},
    {"AT+MQTTCLEAN=0\r\n", "ERROR", 1000, NULL},					//返回error说明当前无mqtt链接 可以进行新的连接

};

AT_URC_t AT_URC_table[AT_COMMAND_ARRAY_SIZE] = {
    {"+MQTTDISCONNECTED", MQTTDISCONNECTED_CallBack},

};

at_err_t AT_SendCmd(AT_Device_t *at_device, const char *cmd, const char *response, uint16_t timeout)
{
    // 参数检查增强
    if (!at_device || !cmd || !at_device->uart_device) {
        return AT_ERR_INVALID_PARAM;
    }

    // 检查指令长度是否超出缓冲区
    uint16_t len = strlen(cmd);
    if (len >= AT_MSG_SIZE) {
        LOG_E("AT command too long!");
        return AT_ERR_INVALID_PARAM;
    }
		
    rt_memset(at_device->uart_device->rx_buffer, 0, AT_MSG_SIZE);
    at_device->uart_device->rx_size = 0;
    at_device->uart_device->rx_flag = 0;
		
		UART_DMA_Transmit(at_device->uart_device , (uint8_t *)cmd , len);

    // 等待响应
    uint32_t start_time = rt_tick_get();
    char *rsp_str = NULL;
    char *error_str = NULL;
//    uint16_t rsp_size = strlen(response);                   //计算应答的长度 需加上\r\n
    uint16_t error_size = strlen("error") + 2;                   //计算应答的长度 需加上\r\n

    while (rt_tick_get() - start_time < timeout) {
        if (at_device->uart_device->rx_flag) 
        {
            rsp_str = strstr((char *)at_device->uart_device->rx_buffer, response);
            error_str = strstr((char *)at_device->uart_device->rx_buffer, "ERROR");

            if (rsp_str!= NULL) 
            {
//                rt_memset(rsp_str , 0 , rsp_size);
//                memmove(rsp_str , rsp_str + rsp_size , rsp_size);               //只清除应答部分的指令，并将剩余指令前移
                return AT_CMD_OK;
            }
            else if (error_str != NULL) 
            {
                rt_memset(error_str , 0 , error_size);
                memmove(error_str , error_str + error_size , error_size);               //只清除error部分的指令，并将剩余指令前移
                return AT_ERR_ACK;
            }
        }
        rt_thread_mdelay(10);
    }
    return AT_ERR_TIMEOUT;
}

/**
 * @brief Register user commands
 */
void user_cmd_register(AT_Device_t *at_device)
{
    AT_Cmd_Register(at_device, "OK", 1000, NULL, -1,
                    "AT+CWJAP=\"%s\",\"%s\"\r\n", DEFAULT_WIFI_SSID, DEFAULT_WIFI_PWD);

    AT_Cmd_Register(at_device, "OK", 1000, NULL, -1,
                    "AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"\r\n");	
	
    AT_Cmd_Register(at_device, "OK", 1000, NULL, -1,
                    "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",
                    DEVICE_NAME, PRODUCT_ID, TOKEN);

//    AT_Cmd_Register(at_device, "OK", 100, NULL, -1,
//                    "AT+MQTTCLIENTID=0,\"%s\"\r\n",
//                    clientId);
	
    AT_Cmd_Register(at_device, "OK", 1000, NULL, -1,
                    "AT+MQTTCONN=0,\"%s\",%d,1\r\n", IP_ADDRESS, PORT_NUMBER);

    AT_Cmd_Register(at_device, "OK", 1000, NULL, -1,
                    "AT+MQTTSUB=0,\"%s\",1\r\n", SET_TOPIC_ALL);
}

struct tm ntpTime = {0};

// 格式化NTP时间为字符串
void Format_Time_String(char *buffer) {
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
           ntpTime.tm_year  , ntpTime.tm_mon, ntpTime.tm_mday,
           ntpTime.tm_hour, ntpTime.tm_min, ntpTime.tm_sec);
}

// 获取NTP时间
uint8_t Get_NTP_Time(AT_Device_t *at_device)
{
    char *timePtr = NULL;
    char timeStr[64] = {0};
    
    // 发送获取NTP时间的命令
    if(AT_SendCmd(at_device, "AT+CIPSNTPTIME?\r\n", "+CIPSNTPTIME:", 5000) != 0) {
        return AT_NTP_SEND_ERROR;
    }
    
    // 查找时间字符串
    timePtr = strstr((char*)at_device->uart_device->rx_buffer, "+CIPSNTPTIME:");
    if(timePtr == NULL) {
        return AT_NTP_ACK_ERROR;
    }
    
    // 跳过"+CIPSNTPTIME:"
    timePtr += strlen("+CIPSNTPTIME:");
		
		rt_thread_mdelay(1000);
		
    char mon[4];
		char weekday[4];
    // 提取时间字符串 (示例格式: "Mon Jan 01 08:00:00 2024")
    sscanf(timePtr, "%3s %3s %2d %2d:%2d:%2d %4d", 
           weekday, mon, &ntpTime.tm_mday, 
           &ntpTime.tm_hour, &ntpTime.tm_min, &ntpTime.tm_sec, 
           &ntpTime.tm_year);
    
    // 转换月份
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for(ntpTime.tm_mon = 0; ntpTime.tm_mon < 12; ntpTime.tm_mon++) {
        if(strncmp(mon, months[ntpTime.tm_mon], 3) == 0) {
            ntpTime.tm_mon += 1; // 转换为1-12
            break;
        }
    }
    
    // 转换星期
    const char *weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    for(ntpTime.tm_wday = 0; ntpTime.tm_wday < 7; ntpTime.tm_wday++) {
        if(strncmp(weekday, weekdays[ntpTime.tm_wday], 3) == 0) {
            break;
        }
    }

    MyRTC_Time[0] = ntpTime.tm_year;
    MyRTC_Time[1] = ntpTime.tm_mon;
    MyRTC_Time[2] = ntpTime.tm_mday;
    MyRTC_Time[3] = ntpTime.tm_hour;
    MyRTC_Time[4] = ntpTime.tm_min;
    MyRTC_Time[5] = ntpTime.tm_sec;
    MyRTC_SetTime();
    return AT_NTP_GET_SUCCESS;
}

at_err_t mqtt_pub(AT_Device_t *at_device, const char *topic, const char *data, uint16_t len) 
{
    char cmd[128];
    uint8_t retry = 0;
    
    // 构造MQTT发布命令
    snprintf(cmd, sizeof(cmd), "AT+MQTTPUBRAW=0,\"%s\",%d,0,0\r\n", topic, len);
	
		if (AT_SendCmd(at_device, cmd, ">", 500) == AT_CMD_OK) 
		{
				if (AT_SendCmd(at_device, data, "OK", 500) == AT_CMD_OK) 
				{
						return AT_MQTT_SEND_SUCCESS;
				}
		}
    return AT_MQTT_SEND_FAIL;
}

uint8_t AT_Cmd_Register(AT_Device_t *at_device, const char *response,
                        uint16_t timeout, void (*callback_response)(void),
                        int insert_count, const char *cmd, ...)
{
    if (!at_device || !cmd || !response)
    {
        LOG_E("Invalid parameters");
        return AT_ERR_INVALID_PARAM;
    }

    if (!at_device->CMD_TABLE)
    {
        LOG_E("CMD_TABLE not initialized");
        return AT_ERR_TABLE_NOT_INIT;
    }

    char cmd_buf[256];
    va_list ap;
    va_start(ap, cmd);
    vsnprintf(cmd_buf, sizeof(cmd_buf), cmd, ap);
    va_end(ap);

    //LOG_I("Registering command: %s", cmd_buf);

    int slot = -1;

    if (insert_count >= 0 && insert_count < AT_COMMAND_ARRAY_SIZE)
    {
        slot = insert_count;
    }
    else
    {
        for (int i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
        {
            if (at_device->CMD_TABLE[i].cmd == NULL)
            {
                slot = i;
                break;
            }
        }
    }

    if (slot == -1)
    {
        //LOG_E("No available slots in command table");
        return AT_ERR_TABLE_FULL;
    }

    size_t cmd_len = strlen(cmd_buf) + 1;
    char *cmd_copy = (char *)malloc(cmd_len);
    if (!cmd_copy)
    {
        //LOG_E("Memory allocation failed for command string");
        return AT_ERR_MEMORY;
    }
    strncpy(cmd_copy, cmd_buf, cmd_len);

    if (at_device->CMD_TABLE[slot].cmd)
    {
        free((char *)at_device->CMD_TABLE[slot].cmd);
    }

    at_device->CMD_TABLE[slot] = (AT_CMD_t){
        .cmd = cmd_copy,
        .response = response,
        .timeout = timeout,
        .callback_response = callback_response};

    //LOG_I("Command registered at slot %d: %s", slot, cmd_copy);
    return AT_CMD_ADD_SUCCESS;
}

/**
 * @brief Register AT device
 * @param at_device Pointer to AT device structure
 * @param uart_device Pointer to UART device structure
 * @param cmd_table Pointer to command table
 * @param urc_table Pointer to URC table
 */
static void at_device_register(AT_Device_t *at_device, uart_device_t *uart_device,
                        AT_CMD_t *cmd_table, AT_URC_t *urc_table)
{
    if (!at_device || !uart_device)
        return;


    at_device->uart_device = uart_device;
    at_device->CMD_TABLE = cmd_table ? cmd_table : AT_Cmd_table;
    at_device->URC_TABLE = urc_table ? urc_table : AT_URC_table;
		at_device->init_step = 0;
}

uint8_t set_ack(AT_Device_t *at_device , char *id)
{
	char ack_buf[64];
	memset(ack_buf,0,64);
	sprintf(ack_buf , "{\"id\": \"%s\",\"code\": 200,\"msg\": \"success\"}" , id);
    return mqtt_pub(at_device , SET_ACK_TOPIC , ack_buf , strlen(ack_buf) );
}

void AT_HW_INIT_TASK(AT_Device_t *at_device)
{
	// 先禁用所有相关中断
	USART_ITConfig(at_device->uart_device->port, USART_IT_RXNE, DISABLE);
	USART_ITConfig(at_device->uart_device->port, USART_IT_IDLE, DISABLE);
	
	// 初始化UART
	My_UART_Init(&AT_DEFAULT_UART_DEVICE);
	
	// 确保DMA配置完成
	rt_thread_mdelay(10);
	
	// 清除中断标志
	USART_ClearITPendingBit(at_device->uart_device->port, USART_IT_IDLE);
	
	// 最后启用中断
	USART_ITConfig(at_device->uart_device->port, USART_IT_IDLE, ENABLE);	
}

static void AT_INIT_TASK(AT_Device_t *at_device)
{
    if(at_device->init_step >= AT_COMMAND_ARRAY_SIZE)
    {
        at_device->init_step = 0;
        at_device->status = AT_IDLE;
        return;
    }
    else if(at_device->CMD_TABLE[at_device->init_step].cmd == NULL)
    {
        at_device->init_step = 0;
        at_device->status = AT_IDLE;
        return;
    }else{
        if(AT_SendCmd(at_device , at_device->CMD_TABLE[at_device->init_step].cmd ,at_device->CMD_TABLE[at_device->init_step].response, at_device->CMD_TABLE[at_device->init_step].timeout) == AT_CMD_OK)
        {
            if(at_device->CMD_TABLE[at_device->init_step].callback_response != NULL)
            {
                at_device->CMD_TABLE[at_device->init_step].callback_response();
            }
            at_device->init_step++;
        }
				else 
				{
//					at_device->init_step = 0;
				}
    }
}

static void AT_IDLE_TASK(AT_Device_t *at_device)
{
    if (!at_device || !at_device->uart_device->rx_flag )
        return;

    (at_device->uart_device->rx_flag) = 0;

    UART_ReadData(at_device->uart_device, at_device->msg_buf, &at_device->uart_device->rx_size);

    for (uint16_t i = 0; i < AT_COMMAND_ARRAY_SIZE && at_device->URC_TABLE[i].urc_msg; i++)
    {
        char *urc_msg = strstr((char *)at_device->msg_buf, at_device->URC_TABLE[i].urc_msg);
        if (urc_msg)
        {
            if (at_device->URC_TABLE[i].callback)
            {
                at_device->URC_TABLE[i].callback(at_device);
            }
            //LOG_I("URC received: %s", at_device->URC_TABLE[i].urc_msg);
            rt_memset(at_device->msg_buf, 0, AT_MSG_SIZE);
            return;
        }
    }

    at_device->status = AT_PARSE;
}

static void AT_PARSE_TASK(AT_Device_t *at_device)
{
		char *data_start = strstr((char *)&at_device->msg_buf[2] , "+MSUB:");
		if( data_start!= NULL)
		{
				char* json_start = strchr(data_start, '{');
				char* json_end = strchr(data_start, '}');
				size_t data_size = 0;
				// 如果没有完整的JSON结构，直接返回
				if(!json_start || !json_end || json_end < json_start) {
						at_device->status = AT_IDLE;
						return;
				}
				char *data_end = strstr(data_start , "\r\n");
				if(data_end != NULL)
				{
					data_size = data_end - data_start;
				}

				cJSON *IPD_js = cJSON_Parse((char *)json_start);
				if(IPD_js == NULL)
				{
						const char *error_ptr = cJSON_GetErrorPtr();
						//LOG_I("Error before: %s\n", error_ptr);
						return ;
				}
				cJSON *id_js = cJSON_GetObjectItemCaseSensitive(IPD_js, "id");
				if(id_js != NULL)
				{
						uint8_t timeout = 5;
						while(set_ack(at_device , id_js->valuestring) != AT_MQTT_SEND_SUCCESS)
						{
								if(timeout-- == 0)
								{
										LOG_I("lot cmd ack fail");
										break;
								}
								rt_thread_mdelay(100);
						}
				}
				else
				{
						cJSON_Delete(IPD_js);
						return ;
				}
				cJSON *params_js = cJSON_GetObjectItem(IPD_js, "params");
				if(params_js != NULL)
				{
								
				}
				else
				{
						cJSON_Delete(params_js);
				}
				cJSON_Delete(IPD_js);

				if(data_size > 0 && data_size < sizeof(AT_DEFAULT_UART_DEVICE.rx_buffer))
				{
					rt_memset(data_start , 0 , data_size);
					rt_memmove(data_start , data_start + data_size , data_size);
					AT_PARSE_TASK(at_device);
				}
			}				
			at_device->status = AT_IDLE; // Return to idle after parsing
}

static void AT_UPDATA_TASK(AT_Device_t *at_device)
{
    // Update task logic can be implemented here
}

/**
 * @brief Main AT device polling function
 * @param at_device Pointer to AT device structure
 */
void AT_poll(AT_Device_t *at_device)
{
    if (!at_device)
        return;

    switch (at_device->status)
    {
				case AT_HW_INIT:
                AT_HW_INIT_TASK(at_device);
                at_device->status = AT_REGISTER;
                break;

        case AT_REGISTER:
            // 注册at设备
            at_device_register(at_device, &AT_DEFAULT_UART_DEVICE, NULL, NULL);
				
						//添加用户自定义at指令
						user_cmd_register(at_device);
				
						//进入at初始化状态
            at_device->status = AT_INIT;
            break;

        case AT_INIT:
            //初始化at设备
            AT_INIT_TASK(at_device);
            break;

        case AT_IDLE:
            AT_IDLE_TASK(at_device);
            break;

        case AT_PARSE:
            AT_PARSE_TASK(at_device);
            break;

        case AT_UPDATA:
            // Update sequence could go here
            at_device->status = AT_IDLE;
            break;

        case AT_GET_NTP:
            Get_NTP_Time(at_device);
            at_device->status = AT_IDLE;
            break;
        default:
            break;
    }
}

