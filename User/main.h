#ifndef _MAIN_H_
#define _MAIN_H_
#include "stm32f10x.h"                  // Device header

/*外设驱动*/
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#include "uart5.h"

/*操作系统*/
#include "log.h"
#include "rtthread.h"
#include "MyRTC.h"

/*应用驱动*/
#include "user_DB.h"

/*c库*/
#include "string.h"


/*协议*/
/*modbus master*/
#define MODBUS_MASTER_ENABLE        0

#if MODBUS_MASTER_ENABLE
#define MODBUS_MASTER_BOUND					115200
#define MODBUS_MASTER_SERIAL				USART3  //serial port
#endif


#define MODBUS_SLAVE_ENABLE        1

#if MODBUS_SLAVE_ENABLE

#define MODBUS_PORT							USART1
#define MODBUS_SLAVE_BOUND					115200
	#define MODBUS_SLAVE_ADDR						1  //device addr

	#define MODBUS_SLAVE_UART1					0

	
	#define MODBUS_SLAVE_UART2					0
   

	#define MODBUS_SLAVE_UART3					1

	
	#define MODBUS_SLAVE_UART4					1

	
	#define MODBUS_SLAVE_UART5					1
	

/*Device*/
#define USE_AT_DEVICE									1

#endif



/*计算公式*/
#define USE_PID										 1

typedef struct
{
	uint8_t ota_flag;
	USART_TypeDef* port;
}ota_t;


#endif
