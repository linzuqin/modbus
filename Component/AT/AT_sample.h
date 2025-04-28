#ifndef AT_SAMPLE_H
#define AT_SAMPLE_H

// Include necessary headers
#include <stdint.h>
#include "AT_Function.h"
#include "list_status.h"
#include "uart.h"
#include "cJSON.h"

#define CAT_PORT USART2
#define CAT_BAUD 115200

#define IP_ADDRESS "mqtts.heclouds.com"
#define PORT_NUMBER 1883
#define PRODUCT_ID "Y6N7IIPWzJ"
#define DEVICE_NAME "shanghai"
#define PASSWORD    "version=2018-10-31&res=products%2FY6N7IIPWzJ%2Fdevices%2Fshanghai&et=1924876800&method=md5&sign=IOfziOfoVwM0w%2BV7tmTwpA%3D%3D"

#define POST_TOPIC		    "$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/post"
#define SET_TOPIC           "$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/set"
#define SET_ACK_TOPIC       "$sys/"PRODUCT_ID"/"DEVICE_NAME"/thing/property/set_reply"

int create_cat1_init_thread(void);

#endif // AT_SAMPLE_H
