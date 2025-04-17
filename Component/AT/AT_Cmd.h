#ifndef _AT_CMD_H_
#define _AT_CMD_H_
#include <stdint.h>
#include <stdbool.h>
#include "stm32f10x.h"                  // Device header

void Device_RST_Soft(void);
void Device_RST_Hard(void);

void Get_IMEI(void);
void Get_CCID(void);
void ERROR_CallBack(void);



#endif