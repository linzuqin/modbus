#include "AT_Cmd.h"
#include "stm32f10x.h"                  // Device header
#include "string.h"
#include "AT_Device.h"
#include "log.h"
#include "rtthread.h"

extern AT_Device_t AT_Device;
extern uint8_t *AT_RxData;

void Device_RST_Soft(void)
{
    AT_Device.status = AT_DISCONNECT;
    AT_Device.init_step = 0;
    LOG_I("Device soft reset initiated\n");
}

void Device_RST_Hard(void)
{
    GPIO_ResetBits(AT_RST_PORT, AT_RST_PIN);
    rt_thread_mdelay(3000);
    GPIO_SetBits(AT_RST_PORT, AT_RST_PIN);
}

void ERROR_CallBack(void)
{
    static uint8_t error_count = 0;
    error_count ++;
    if(error_count > 5)
    {
        error_count = 0;
        if(AT_Device.status == AT_DISCONNECT)
        {
            Device_RST_Soft();
            LOG_I("Device soft reset initiated\n");
        }
        else
        {
            Device_RST_Hard();
            LOG_I("Device hard reset initiated\n");
        }
    }
    else
    {
        LOG_I("ack error :%d\n",error_count);
    }
}

void Get_IMEI(void)
{
    memcpy(AT_Device.IMEI, &AT_RxData[0], 15);
    LOG_I("IMEI: %s\n", AT_Device.IMEI);
}

void Get_CCID(void)
{
    memcpy(AT_Device.ICCID, &AT_RxData[0], 20);
    LOG_I("IMEI: %s\n", AT_Device.IMEI);
}