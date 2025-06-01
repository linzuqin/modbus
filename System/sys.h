#ifndef _SYS_H_
#define _SYS_H_
#include "stm32f10x.h"                  // Device header

/*外设驱动*/
#include "uart.h"
#include "MyRTC.h"
#include "GPIO.h"
#include "PWM.h"
#include "iwdog.h"
#include "ADC.h"
#include "TIM.h"


/*操作系统*/
#include "log.h"
#include "rtthread.h"

/*应用驱动*/
#include "user_DB.h"
#include "mb.h"
#include "slave_mb_app.h"
#include "AT_Function.h"
#include "MPU6050.h"
#include "ds18b20.h"
#include "cJSON.h"
#include "string_Optimize.h"

/*c库*/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <stdint.h>
#include <stdbool.h>
#include "math.h"
#include <stdarg.h>
#include "time.h"

/*project version*/
#define Version											"1.4.3"

/*协议*/
/*modbus master*/
#define MODBUS_MASTER_ENABLE        0
#if MODBUS_MASTER_ENABLE
#define MB_M_DEVICE				            uart3_device  //serial port
#endif

#define MODBUS_MASTER_SENSOR_ENABLE        0
#if MODBUS_MASTER_SENSOR_ENABLE
#define MODBUS_MASTER_SENSOR_DEVICE        uart3_device
#endif

#define MODBUS_SLAVE_ENABLE        0
#if MODBUS_SLAVE_ENABLE
#define MB_S_DEVICE_1							uart1_device
#define MODBUS_SLAVE_ADDR						1  //device addr
#define MODBUS_SLAVE_BOUND					115200 //baudrate
#endif

/*计算公式*/
#define USE_PID										 0

void rt_hw_us_delay(uint32_t us) ;


#endif
