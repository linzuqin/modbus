#ifndef _AT_DEVICE_H_
#define _AT_DEVICE_H_
#include "AT_Function.h"
#include "log.h"
#include "rtthread.h"


#define AT_CONNECT      0
#define AT_DISCONNECT   1
#define AT_IDEL         2
#define AT_SEND         3

#define AT_RST_PORT     GPIOC
#define AT_RST_PIN      GPIO_Pin_5


#define AT_DATA_START_BIT		2

#define AT_COMMAND_ARRAY_SIZE   32


void AT_START(void);


#endif
