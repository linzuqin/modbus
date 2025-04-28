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

#define AT_CONNECT      0
#define AT_DISCONNECT   1
#define AT_IDEL         2
#define AT_SEND         3

#define AT_RST_PORT     GPIOC
#define AT_RST_PIN      GPIO_Pin_5

#define AT_COMMAND_ARRAY_SIZE 32

typedef struct
{
    const char *cmd;                            // AT command string
    const char *response;                       // Expected response string
    uint16_t timeout;                     // Timeout in milliseconds
    void (*ack_right_response)(void);     // Expected response string
    void (*ack_err_response)(void);       // Expected response string

} AT_Command_t;

// 定义 AT 设备结构体
typedef struct {
    uint8_t status;                 // 设备状态
    uint8_t init_step;              // 初始化步骤
    uint8_t *rx_buf;                // 接收缓冲区指针
    uint8_t *tx_buf;                // 发送缓冲区指针
    uint8_t *rx_flag;               // 接收标志指针
    USART_TypeDef *PORT;            // 串口端口
    uint32_t Bound;                 // 波特率
    AT_Command_t *init_cmd;         // AT 命令数组指针
    AT_Command_t *run_cmd;          // AT 命令数组指针
    uint16_t status_up_interval;	//状态更新间隔
    uint8_t is_status_up;           //状态更新标志 

} AT_Device_t;


typedef struct
{
    char *urc_msg;              // AT command string
    void (*response)(AT_Device_t *at_device);     // Expected response string
} AT_URC_t;


void Device_RST_Soft(AT_Device_t *at_device);
void Device_RST_Hard(void);

uint8_t AT_SendCmd( AT_Device_t *at_device , const char *cmd , const char *response , uint16_t timeout);
void at_device_register(AT_Device_t *at_device  , USART_TypeDef *USARTx , uint32_t bound , uint8_t **rx_buffer , uint8_t *rx_flag , AT_Command_t *init_cmd , AT_Command_t *run_cmd);
uint8_t AT_Cmd_Regsiter(AT_Command_t *at_cmd_array , const char *response, uint16_t timeout, void (*ack_right_response)(void), void (*ack_err_response)(void) , const char *cmd, ...);


#endif