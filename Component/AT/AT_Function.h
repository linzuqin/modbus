#ifndef _AT_FUNCTION_H_
#define _AT_FUNCTION_H_
#include "stm32f10x.h"                  // Device header
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "log.h"
#include "rtthread.h"
#include <stdarg.h>
#include <stdio.h>
#include "uart.h"
#include "GPIO.h"

#define AT_RST_PORT     GPIOC
#define AT_RST_PIN      GPIO_Pin_5

#define AT_DEFAULT_UART_DEVICE  uart3_device

#define AT_COMMAND_ARRAY_SIZE 32

#define NTP_SERVER "time.windows.com"

#define DEFAULT_WIFI_SSID "main" // 默认WiFi SSID
#define DEFAULT_WIFI_PWD  "12345678" // 默认WiFi密码

#define IP_ADDRESS "mqtts.heclouds.com"
#define PORT_NUMBER 1883
#define PRODUCT_ID "9le3c1Ex8I"
#define DEVICE_NAME "channel_test"
#define TOKEN    "version=2018-10-31&res=products%2F9le3c1Ex8I%2Fdevices%2Fchannel_test&et=1924876800&method=md5&sign=QXNNJE4w3QFoCC%2FIbqaH5w%3D%3D"

#define POST_TOPIC		    "$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/post"
#define SET_TOPIC_ALL     "$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/#"

#define SET_TOPIC           "$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/set"
#define SET_ACK_TOPIC       "$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/set_reply"

typedef enum
{
    AT_NO_REGISTER = 0,
    AT_REGISTERED,
    AT_DISCONNECT,
    AT_CONNECT,
    AT_IDEL,
    AT_PARSE,
    AT_UPDATA,
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
    void (*callback)(void);     // Expected response string
} AT_URC_t;

// 定义 AT 设备结构体
typedef struct {
    AT_STATUS_t status;                 // 设备状态
    uint8_t init_step;              // 初始化步骤
    uint8_t *rx_buf;                // 接收缓冲区指针
    uint8_t *rx_flag;               // 接收标志指针
    USART_TypeDef *PORT;            // 串口端口
    uint32_t Bound;                 // 波特率
    AT_CMD_t *CMD_TABLE;         // AT 命令数组指针
    AT_URC_t *URC_TABLE;         // AT 命令数组指针

} AT_Device_t;



void Device_RST_Soft(AT_Device_t *at_device);
void Device_RST_Hard(void);

uint8_t AT_SendCmd( AT_Device_t *at_device , const char *cmd , const char *response , uint16_t timeout); 
void at_device_register(AT_Device_t *at_device  , uart_device_t *uart_device , AT_CMD_t *cmd_table , AT_URC_t *urc_table);
uint8_t AT_Cmd_Regsiter(AT_Device_t *at_device , const char *response, uint16_t timeout, void (*callback_response)(void), int insert_count , const char *cmd, ...);


#endif