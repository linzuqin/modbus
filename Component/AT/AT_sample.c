#include "AT_sample.h"
#include <rtthread.h>
#include "GPIO.h" 

/*定义AT指令集*/
AT_Command_t CAT_Init_Cmd[AT_COMMAND_ARRAY_SIZE] ={
    {"AT\r\n",          "OK",   100 , NULL , NULL},
    {"AT+RESET\r\n",    "NITZ", 2000 , NULL , NULL},
};


AT_Command_t CAT_Run_Cmd[AT_COMMAND_ARRAY_SIZE] = {
};

// Static initialization for cat1
#define cat1_stack_size	4096
static struct rt_thread cat1_thread;
static uint8_t cat1_stack[cat1_stack_size];

#define cat1_URC_stack_size	512
static struct rt_thread cat1_URC_thread;
static uint8_t cat1_URC_stack[cat1_URC_stack_size];

static AT_Device_t CAT_Device;

extern AT_Command_t CAT_Init_Cmd[AT_COMMAND_ARRAY_SIZE];
extern AT_Command_t CAT_Run_Cmd[AT_COMMAND_ARRAY_SIZE];
extern AT_URC_t AT_URC_Msg[AT_COMMAND_ARRAY_SIZE];
extern uint16_t MyRTC_Time[];

static void CCLK_CallBack(void)
{
    char *time_data = strstr((char *)CAT_Device.rx_buf, "+CCLK: ");
    if (time_data != NULL) {
        time_data += strlen("+CCLK: ");
        // Assuming RTC_Time_Array is a predefined array to store RTC time
        MyRTC_Time[0] = (time_data[0] - '0') * 10 + (time_data[1] - '0'); // Year
        MyRTC_Time[1] = (time_data[3] - '0') * 10 + (time_data[4] - '0'); // Month
        MyRTC_Time[2] = (time_data[6] - '0') * 10 + (time_data[7] - '0'); // Day
        MyRTC_Time[3] = (time_data[9] - '0') * 10 + (time_data[10] - '0'); // Hour
        MyRTC_Time[4] = (time_data[12] - '0') * 10 + (time_data[13] - '0'); // Minute
        MyRTC_Time[5] = (time_data[15] - '0') * 10 + (time_data[16] - '0'); // Second
    }
    MyRTC_SetTime();
    rt_kprintf("RTC Time: %d-%02d-%02d %02d:%02d:%02d\n", MyRTC_Time[0], MyRTC_Time[1], MyRTC_Time[2], MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5]);

    rt_kprintf("Unix Time: %d\n", Unix_Time);

    rt_memset(CAT_Device.rx_buf , 0 , sizeof(CAT_Device.rx_buf));
}

/*下发数据解析*/
uint8_t *get_msg(void)//UART1_RxData
{
	uint8_t *buf_ptr = NULL;
	uint16_t i;

	for(i=0;CAT_Device.rx_buf[i]!='{';i++);
	buf_ptr = CAT_Device.rx_buf +i;
	return buf_ptr;
}

/*数据发送*/
void mqtt_send(char *topic , char *data)
{
	//AT_SendCmd("\r\nAT+MPUBEX=\"%s\",0,0,%d\r\n",topic,1000);
	char cmd[256];
	sprintf(cmd , "AT+MPUBEX=\"%s\",0,0,%d\r\n" , topic , strlen(data));
	if(AT_SendCmd(&CAT_Device,cmd , ">" ,1000) == 1)return;
	AT_SendCmd(&CAT_Device,data,"OK",1000);
}

/*onenet数据组包*/
uint16_t mqtt_msg(char *buf)
{
	char text[128];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{\"id\":\"123\",\"params\":{");

//   memset(text, 0, sizeof(text));
//	sprintf(text, "\"C_1\":{\"value\":%d},", (int)((usSRegInBuf[23] >> 8)/2.55));
//	strcat(buf, text);

	strcat(buf, "}}");

	return strlen(buf);
}

void AT_UpStatus(void)
{
    char data[256];
    uint16_t len = mqtt_msg(data);
    if(len > 0)
    {
        mqtt_send(POST_TOPIC , data);
        return ;
    }
    else
    {
        LOG_I("Failed to create MQTT message\n");
        return ;
    }
}

void AT_CHECK(void)
{
	if(AT_SendCmd(&CAT_Device,"AT*SIMDETEC=1\r\n" , "OK" ,1000) == 1)return;
}

void set_ack(char *id)
{
	char ack_buf[64];
	memset(ack_buf,0,64);
	sprintf(ack_buf , "{\"id\": \"%s\",\"code\": 200,\"msg\": \"success\"}" , id);
    mqtt_send(SET_ACK_TOPIC , ack_buf);
}

void AT_Parase(void)
{
	if(*(CAT_Device.rx_flag) == 1)
	{	
		*(CAT_Device.rx_flag) = 0;
        if(strstr((char *)CAT_Device.rx_buf , SET_TOPIC) != NULL)
        {
            uint8_t *buf_ptr = NULL;
            buf_ptr = get_msg();
            if(buf_ptr == NULL)
            {
                return ;
            }

            cJSON *IPD_js = cJSON_Parse((char *)buf_ptr);
            if(IPD_js == NULL)
            {
                const char *error_ptr = cJSON_GetErrorPtr();
                LOG_I("Error before: %s\n", error_ptr);
                return ;
            }
            cJSON *id_js = cJSON_GetObjectItem(IPD_js, "id");
            if(id_js != NULL)
            {
                set_ack(id_js->valuestring);
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
        }
    }
}

void cat1_init_thread(void *parameter) {
    /*状态初始化*/
    static uint8_t *step = &CAT_Device.init_step;
    CAT_Device.status = AT_DISCONNECT;

    /*初始化复位引脚*/
    MyGPIO_Init(AT_RST_PORT, AT_RST_PIN, GPIO_Mode_Out_PP); // Initialize GPIO for reset

    /*AT设备注册*/
    at_device_register(&CAT_Device , &AT_DEVICE ,CAT_Init_Cmd , CAT_Run_Cmd);
	
    /*添加自定义指令（获取NTP时间）*/
    AT_Cmd_Regsiter(CAT_Init_Cmd , "OK" , 1000 , NULL , NULL , "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n");
    AT_Cmd_Regsiter(CAT_Init_Cmd , "OK" , 1000 , NULL , NULL , "AT+SAPBR=3,1,\"APN\",\"\"\r\n");
    AT_Cmd_Regsiter(CAT_Init_Cmd , "OK" , 1000 , NULL , NULL , "AT+SAPBR=1,1\r\n");
    AT_Cmd_Regsiter(CAT_Init_Cmd , "OK" , 1000 , NULL , NULL , "AT+CNTPCID=1\r\n");
    AT_Cmd_Regsiter(CAT_Init_Cmd , "+CNTP:1" , 1000 , NULL , NULL , "AT+CNTP=\"%s\"\r\n", NTP_SERVER);
    AT_Cmd_Regsiter(CAT_Init_Cmd , "OK" , 1000 , CCLK_CallBack , NULL , "AT+CCLK?\r\n");


    /*添加自定义指令（连接平台）*/
    AT_Cmd_Regsiter(CAT_Init_Cmd , "OK" , 1000 , NULL , NULL , "AT+MCONFIG=\"%s\",\"%s\",\"%s\"\r\n",DEVICE_NAME , PRODUCT_ID , PASSWORD);
    AT_Cmd_Regsiter(CAT_Init_Cmd , "CONNECT OK" , 1000 , NULL , NULL , "AT+MIPSTART=\"%s\",%d\r\n",IP_ADDRESS , PORT_NUMBER);
    AT_Cmd_Regsiter(CAT_Init_Cmd , "CONNACK OK" , 1000 , NULL , NULL , "AT+MCONNECT=1,120\r\n");
    AT_Cmd_Regsiter(CAT_Init_Cmd , "SUBACK" , 1000 , NULL , NULL , "AT+MSUB=\"%s\",0\r\n" , SUB_TOPIC);

    /*获取指令长度*/
    uint16_t cmd_size = sizeof(CAT_Init_Cmd) / sizeof(AT_Command_t);

    while (1) {

        // Perform initialization tasks for cat1
        if(CAT_Device.status == AT_DISCONNECT)
        {
            if (AT_SendCmd(&CAT_Device , CAT_Device.init_cmd[*step].cmd , CAT_Device.init_cmd[*step].response , CAT_Device.init_cmd[*step].timeout) != 0)
            {
                if(CAT_Device.init_cmd[*step].ack_err_response != NULL)
                {
                    CAT_Device.init_cmd[*step].ack_err_response();
                }
            }
            else
            {
                if(CAT_Device.init_cmd[*step].ack_right_response != NULL)
                {
                    CAT_Device.init_cmd[*step].ack_right_response();
                }
                (*step) ++;

                if(*step >= cmd_size)
                {
										CAT_Device.status = AT_IDEL;
                }
                else if(CAT_Device.init_cmd[*step].cmd == NULL)
                {
										CAT_Device.status = AT_IDEL;
                }
            }
						rt_thread_mdelay(1000);  // Delay for demonstration purposes
				}
        else
        {
            if(CAT_Device.is_status_up == 1)
            {
                CAT_Device.is_status_up = 0;
            }
            rt_thread_mdelay(10);  // Delay for demonstration purposes
        }
        
    }
}

void cat1_urc_thread(void *params)
{
    while(1)
    {
        for(uint8_t i = 0; i < AT_COMMAND_ARRAY_SIZE; i++)
        {
            if(AT_URC_Msg[i].urc_msg != NULL)
            {
                if(strstr((char *)CAT_Device.rx_buf , AT_URC_Msg[i].urc_msg) != NULL)
                {
                    if(AT_URC_Msg[i].response != NULL)
                    {
                        AT_URC_Msg[i].response(&CAT_Device);
                    }
                }
            }
        }
				rt_thread_mdelay(100);  // Delay for demonstration purposes

    }
}

// Create a statically initialized thread for cat1
int create_cat1_init_thread(void) {
    rt_err_t result = rt_thread_init(&cat1_thread,
                                     "cat1_init",
                                     cat1_init_thread,
                                     RT_NULL,
                                     cat1_stack,
                                     cat1_stack_size,
                                     5,   // Thread priority
                                     100);  // Time slice
    if (result == RT_EOK) {
        if (rt_thread_startup(&cat1_thread) != RT_EOK) {
            rt_kprintf("Failed to start cat1_thread\n");
            return -1; // Return error code if thread startup fails
        }
    } else {
        rt_kprintf("Failed to initialize cat1_thread\n");
        return -1; // Return error code if thread initialization fails
    }
		
    result = rt_thread_init(&cat1_URC_thread,
                            "cat1_urc_init",
                            cat1_urc_thread,
                            RT_NULL,
                            cat1_URC_stack,
                            cat1_URC_stack_size,
                            5,   // Thread priority
                            100);  // Time slice
    if (result == RT_EOK) {
        if (rt_thread_startup(&cat1_URC_thread) != RT_EOK) {
            rt_kprintf("Failed to start cat1_URC_thread\n");
            return -1; // Return error code if thread startup fails
        }
    } else {
        rt_kprintf("Failed to initialize cat1_URC_thread\n");
        return -1; // Return error code if thread initialization fails
    }
    return 0; // Return success code
}
