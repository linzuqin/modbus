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

typedef struct
{
    const char *cmd;                            // AT command string
    const char *response;                       // Expected response string
    uint16_t timeout;                     // Timeout in milliseconds
    void (*ack_right_response)(void);     // Expected response string
    void (*ack_err_response)(void);       // Expected response string

} AT_Command_t;

typedef struct
{
    char *urc_msg;              // AT command string
    void (*response)(void);     // Expected response string
} AT_URC_t;

typedef struct{
    uint8_t status;
    uint8_t init_step;
    char IMEI[15];
    char ICCID[20];
    uart_device_t *at_uart_device;
}AT_Device_t;

void Device_RST_Soft(void);
void Device_RST_Hard(void);

void Get_IMEI(void);
void Get_CCID(void);
void ERROR_CallBack(void);
uint8_t AT_SendCmd(const char *cmd , const char *response, uint16_t timeout);
void at_device_register(USART_TypeDef *USARTx , uint32_t bound);
uint8_t AT_Cmd_Regsiter(AT_Command_t *AT_Command_array , const char *response, uint16_t timeout, void (*ack_right_response)(void), void (*ack_err_response)(void) , const char *cmd, ...);


#endif