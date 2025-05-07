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
#include "mb.h"
#include "slave_mb_app.h"
#include "AT_Function.h"
/*c库*/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <stdint.h>
#include <stdbool.h>

/*project version*/
#define Version											"1.4.2"

/*协议*/
/*modbus master*/
#define MODBUS_MASTER_ENABLE        1
#if MODBUS_MASTER_ENABLE
#define MB_M_DEVICE				            uart3_device  //serial port
#endif

#define MODBUS_MASTER_SENSOR_ENABLE        1
#if MODBUS_MASTER_SENSOR_ENABLE
#define MODBUS_MASTER_SENSOR_DEVICE        uart3_device
#endif

#define MODBUS_SLAVE_ENABLE        1
#if MODBUS_SLAVE_ENABLE
#define MB_S_DEVICE_1							uart1_device
#define MODBUS_SLAVE_ADDR						1  //device addr
#define MODBUS_SLAVE_BOUND					115200 //baudrate
#endif


/*Device*/
#define AT_DEVICE                                uart1_device
#define USE_AT_SAMPLE									1



/*计算公式*/
#define USE_PID										 1

#define uart1_rx_size    256 // Size of the UART receive buffer

#define uart2_rx_size    256 // Size of the UART receive buffer

#define uart3_rx_size    256 // Size of the UART receive buffer

#define uart4_rx_size    256 // Size of the UART receive buffer

#define uart5_rx_size    256 // Size of the UART receive buffer

#endif
