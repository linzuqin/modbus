#ifndef _MAIN_H_
#define _MAIN_H_
#include "stm32f10x.h"                  // Device header

/*外设驱动*/
#include "uart.h"
#include "MyRTC.h"
#include "GPIO.h"
#include "TIM.h"
#include "iwdog.h"


/*操作系统*/
#include "log.h"
#include "rtthread.h"

/*应用驱动*/
#include "user_DB.h"
#include "AT_Device.h"
#include "mb.h"
#include "slave_mb_app.h"
#include "master_mb_app.h"

/*c库*/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"


/*project version*/
#define Version											"1.4.2"

/*协议*/
/*modbus master*/
#define MODBUS_MASTER_ENABLE        1

#if MODBUS_MASTER_ENABLE

#define MODBUS_MASTER_SERIAL				USART3  //serial port
#define MODBUS_MASTER_BOUND					115200

#endif


#define MODBUS_SLAVE_ENABLE        1

#if MODBUS_SLAVE_ENABLE

#define MODBUS_PORT							USART1
#define MODBUS_SLAVE_BOUND					115200
#define MODBUS_SLAVE_ADDR						1  //device addr

#endif


/*Device*/
#define USE_AT_DEVICE									1

/*计算公式*/
#define USE_PID										 1

typedef struct
{
	uint8_t ota_flag;
	USART_TypeDef* port;
}ota_t;


#endif
