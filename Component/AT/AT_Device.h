#ifndef _AT_DEVICE_H_
#define _AT_DEVICE_H_
#include <stdint.h>
#include <stdbool.h>
#include "stm32f10x.h"                  // Device header

#define AT_CONNECT      0
#define AT_DISCONNECT   1
#define AT_IDEL         2
#define AT_SEND         3

#define AT_RST_PORT     GPIOC
#define AT_RST_PIN      GPIO_Pin_5

typedef struct
{
    char *cmd;          // AT command string
    char *response;     // Expected response string
    uint16_t timeout;   // Timeout in milliseconds
} AT_Command_t;

typedef struct
{
    char *urc_msg;              // AT command string
    void (*response)(void);     // Expected response string
} AT_URC_t;

typedef struct{
    uint8_t status;
}AT_Device_t;
void AT_START(void);


#endif
