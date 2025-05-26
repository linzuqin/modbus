#ifndef _AT_FUNCTION_H_
#define _AT_FUNCTION_H_

#include "sys.h"

#define AT_RST_PORT     GPIOC
#define AT_RST_PIN      GPIO_Pin_5

#define AT_DEFAULT_UART_DEVICE  uart_devices[0]

#define AT_COMMAND_ARRAY_SIZE 16
#define AT_MSG_SIZE 256


/*平台连接参数宏定义*/
#define NTP_SERVER "time.windows.com"

#define DEFAULT_WIFI_SSID 					"main" // 默认WiFi SSID
#define DEFAULT_WIFI_PWD  					"12345678" // 默认WiFi密码

#define IP_ADDRESS 									"mqtts.heclouds.com"
#define PORT_NUMBER 								1883
#define PRODUCT_ID 									"2Its5wq8a3"
#define DEVICE_NAME 								"lot_device"
#define TOKEN    										"version=2018-10-31&res=products%2F2Its5wq8a3%2Fdevices%2Flot_device&et=1924876800&method=md5&sign=yEio49aeh%2BTf3XPlD8OyPQ%3D%3D"

#define POST_TOPIC		    					"$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/post"
#define SET_TOPIC_ALL     					"$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/#"

#define SET_TOPIC          				 	"$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/set"
#define SET_ACK_TOPIC       				"$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/set_reply"

typedef enum
{
    AT_HW_INIT = 0,
    AT_REGISTER,
    AT_INIT,
    AT_DISCONNECT,
    AT_CONNECT,
    AT_IDLE,
    AT_PARSE,
    AT_UPDATA,
    AT_GET_NTP,
} AT_STATUS_t;

typedef struct
{
    const char *cmd;                            // AT command string
    const char *response;                       // Expected response string
    uint16_t timeout;                     // Timeout in milliseconds
    void (*callback_response)(void);     // Expected response string
} AT_CMD_t;

typedef struct
{
    char *urc_msg;              // AT command string
    void (*callback)(void *a);     // Expected response string
} AT_URC_t;

// 定义 AT 设备结构体
typedef struct {
    AT_STATUS_t status;                 // 设备状态
    AT_CMD_t *CMD_TABLE;                // AT 命令数组指针
    AT_URC_t *URC_TABLE;                // AT 命令数组指针
    uart_device_t *uart_device;         // UART 设备指针
    uint8_t *msg_buf;                   // 消息缓冲区
		uint8_t init_step;									//初始化步骤
} AT_Device_t;


typedef enum
{
	/*AT指令相关错误类型*/
	AT_CMD_OK = 0,                          //AT指令发送成功
	AT_ERR_INVALID_PARAM,                   //AT指令参数错误
	AT_ERR_TIMEOUT,                         //AT指令应答超时
	AT_ERR_ACK,                             //AT指令应答错误
	
	/*AT指令列表相关错误类型*/
  AT_CMD_ADD_SUCCESS,                     //AT指令添加成功
	AT_ERR_TABLE_NOT_INIT,                  //AT设备指令集未初始化
	AT_ERR_TABLE_FULL,                      //AT指令集已满
	AT_ERR_MEMORY,                          //内存不足
	
	/*NTP相关错误类型*/
  AT_NTP_GET_SUCCESS,                     //NTP指令获取成功
	AT_NTP_SEND_ERROR,                      //NTP获取指令发送失败
  AT_NTP_ACK_ERROR,                       //NTP指令应答失败
	AT_NTP_PARSE_ERROR,
	
	/*MQTT相关错误类型*/
	AT_MQTT_SEND_SUCCESS,                   //MQTT指令上报成功
	AT_MQTT_SEND_FAIL,                      //MQTT指令上报失败

}at_err_t;

void AT_poll(AT_Device_t *at_device);
uint8_t AT_Cmd_Register(AT_Device_t *at_device, const char *response,
                        uint16_t timeout, void (*callback_response)(void),
                        int insert_count, const char *cmd, ...);

#endif