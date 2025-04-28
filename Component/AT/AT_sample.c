#include "AT_sample.h"
#include <rtthread.h>
#include "GPIO.h" 

// Static initialization for cat1
#define cat1_stack_size	1024
static struct rt_thread cat1_thread;
static uint8_t cat1_stack[cat1_stack_size];

#define cat1_URC_stack_size	1024
static struct rt_thread cat1_URC_thread;
static uint8_t cat1_URC_stack[cat1_URC_stack_size];

static AT_Device_t CAT_Device;

extern uart_device_t uart2_device;
extern AT_Command_t CAT_Init_Cmd[AT_COMMAND_ARRAY_SIZE];
extern AT_Command_t CAT_Run_Cmd[AT_COMMAND_ARRAY_SIZE];
extern AT_URC_t AT_URC_Msg[AT_COMMAND_ARRAY_SIZE];

static list_status_t* AT_IDLE_list = NULL;
static list_status_t* AT_CHECK_list = NULL;

static list_status_t* AT_UP_list = NULL;
static list_status_t* AT_current = NULL;

uint8_t *get_msg(void)//UART1_RxData
{
	uint8_t *buf_ptr = NULL;
	uint16_t i;

	for(i=0;CAT_Device.rx_buf[i]!='{';i++);
	buf_ptr = CAT_Device.rx_buf +i;
	return buf_ptr;
}

void mqtt_send(char *data)
{
	//AT_SendCmd("\r\nAT+MPUBEX=\"%s\",0,0,%d\r\n",topic,1000);
	char cmd[256];
	sprintf(cmd , "AT+MPUBEX=\"%s\",0,0,%d\r\n" , SET_ACK_TOPIC , strlen(data));
	if(AT_SendCmd(&CAT_Device,cmd , ">" ,1000) == 1)return;
	AT_SendCmd(&CAT_Device,data,"OK",1000);
}


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
        mqtt_send(data);
        return ;
    }
    else
    {
        LOG_I("Failed to create MQTT message\n");
        return ;
    }
}

void AT_To_IDLE(void)
{
    list_status_remove(AT_UP_list);
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
    mqtt_send(ack_buf);
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
    static uint8_t *step = &CAT_Device.init_step;
    CAT_Device.status = AT_DISCONNECT;

    MyGPIO_Init(AT_RST_PORT, AT_RST_PIN, GPIO_Mode_Out_PP); // Initialize GPIO for reset
//    My_UART_Init(CAT_Device.PORT, CAT_Device.Bound); // Initialize UART for cat1
    at_device_register(&CAT_Device , CAT_PORT , CAT_BAUD , &(uart2_device.rx_buffer) , &uart2_device.rx_flag ,CAT_Init_Cmd , CAT_Run_Cmd);
	
    AT_Cmd_Regsiter(CAT_Init_Cmd , "OK" , 1000 , NULL , NULL , "AT+MCONFIG=\"%s\",\"%s\",\"%s\"\r\n",DEVICE_NAME , PRODUCT_ID , PASSWORD);
    AT_Cmd_Regsiter(CAT_Init_Cmd , "CONNECT OK" , 1000 , NULL , NULL , "AT+MIPSTART=\"%s\",%d\r\n",IP_ADDRESS , PORT_NUMBER);
    AT_Cmd_Regsiter(CAT_Init_Cmd , "CONNACK OK" , 1000 , NULL , NULL , "AT+MCONNECT=1,120\r\n");
    AT_Cmd_Regsiter(CAT_Init_Cmd , "SUBACK" , 1000 , NULL , NULL , "AT+MSUB=\"%s\",0\r\n" , SET_TOPIC);
    uint16_t cmd_size = sizeof(CAT_Init_Cmd) / sizeof(AT_Command_t);

    //AT_Cmd_Regsiter(CAT_Run_Cmd , ">" , 1000 , NULL , NULL , "AT+MPUBEX=\"%s\",0,0,%d\r\n" , SET_ACK_TOPIC);

    AT_IDLE_list = list_status_create("AT IDLE" , NULL , AT_Parase , NULL);
    AT_UP_list = list_status_create("AT UP" , NULL , AT_UpStatus , AT_To_IDLE);
		AT_CHECK_list = list_status_create("AT CHECK" , NULL , AT_CHECK , NULL);
		list_status_add(&AT_IDLE_list , AT_CHECK_list);
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
                list_status_add(&AT_IDLE_list , AT_UP_list);
            }
            list_poll(AT_IDLE_list , &AT_current);
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
