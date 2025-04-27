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

typedef struct{
    uint8_t status;
    uint8_t init_step;
    char IMEI[15];
    char ICCID[20];
    uint8_t *rx_buf;
    uint8_t *tx_buf;
    uint8_t * rx_flag;
}AT_Device_t;


typedef struct
{
    char *urc_msg;              // AT command string
    void (*response)(AT_Device_t *at_device);     // Expected response string
} AT_URC_t;


void Device_RST_Soft(AT_Device_t *at_device);
void Device_RST_Hard(void);

void ERROR_CallBack(void);
uint8_t AT_SendCmd( AT_Device_t *at_device , const char *cmd , const char *response, uint16_t timeout);
void at_device_register(USART_TypeDef *USARTx , uint32_t bound , AT_Device_t *at_device , uint8_t *rx_buffer );
uint8_t AT_Cmd_Regsiter(AT_Command_t *AT_Command_array , const char *response, uint16_t timeout, void (*ack_right_response)(void), void (*ack_err_response)(void) , const char *cmd, ...);
void at_list_poll(void);
void at_list_init(void);


#endif