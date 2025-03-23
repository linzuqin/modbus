/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-01-30     armink       the first version
 * 2018-08-27     Murphy       update log
 */

#include <rtthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
//#include <finsh.h>
#include <fal.h>
#include <ymodem.h>
#define RT_APP_PART_ADDR 0x08020000
//#define DBG_ENABLE
//#define DBG_SECTION_NAME               "ymodem"
//#ifdef OTA_DOWNLOADER_DEBUG
//#define DBG_LEVEL                      DBG_LOG
//#else
//#define DBG_LEVEL                      DBG_INFO
//#endif
//#define DBG_COLOR
//#include <rtdbg.h>
#include "main.h"
extern ota_t ota;

#define PKG_USING_YMODEM_OTA

#ifdef PKG_USING_YMODEM_OTA

#define DEFAULT_DOWNLOAD_PART "download"

static char stack_ymd[768];
static struct rt_thread ymd_th;
static void ymodem(void *param);

static size_t update_file_total_size, update_file_cur_size;
static const struct fal_partition * dl_part = RT_NULL;
static uint8_t enable_output_log = 0;

static enum rym_code ymodem_on_begin(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    char *file_name, *file_size;

    /* calculate and store file size */
    file_name = (char *)&buf[0];
    file_size = (char *)&buf[rt_strlen(file_name) + 1];
    update_file_total_size = atol(file_size);
    if (enable_output_log) {rt_kprintf("Ymodem file_size:%d\n", update_file_total_size);}

    update_file_cur_size = 0;

    /* Get download partition information and erase download partition data */
    if (update_file_total_size > dl_part->len)
    {
        if (enable_output_log) {LOG_E("Firmware is too large! File size (%d), '%s' partition size (%d)", update_file_total_size, dl_part->name, dl_part->len);}
        return RYM_CODE_CAN;
    }

    if (enable_output_log) {LOG_I("Start erase. Size (%d)", update_file_total_size);}

    /* erase DL section */
    if (fal_partition_erase(dl_part, 0, update_file_total_size) < 0)
    {
        if (enable_output_log) {LOG_E("Firmware download failed! Partition (%s) erase error!", dl_part->name);}
        return RYM_CODE_CAN;
    }

    return RYM_CODE_ACK;
}

static enum rym_code ymodem_on_data(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    /* write data of application to DL partition  */
    if (fal_partition_write(dl_part, update_file_cur_size, buf, len) < 0)
    {
        if (enable_output_log) {LOG_E("Firmware download failed! Partition (%s) write data error!", dl_part->name);}
        return RYM_CODE_CAN;
    }

    update_file_cur_size += len;

    return RYM_CODE_ACK;
}

struct ymd_device dev;
static rt_uint8_t ymodem_qp[(200+4)*1];/* 消息队列内存池,大小设置为4条 MB_SER_PDU_SIZE_MAX 消息*/
struct rt_messagequeue ymodem_mq;/* 消息队列控制块 */
void ymodem_ota(uint8_t argc, char **argv)
{
    struct rym_ctx rctx;
//    const char str_usage[] = "Usage: ymodem_ota -p <partiton name> -t <device name>.\n";
//    int i;
    char* recv_partition = DEFAULT_DOWNLOAD_PART;
    enable_output_log = 0;
    
    
    if ((dl_part = fal_partition_find(recv_partition)) == RT_NULL)
    {
        rt_kprintf("Partition (%s) find error!\n", recv_partition);
        return;
    }
//    if (dev != rt_console_get_device()) {enable_output_log = 1;}
    rt_mq_init(&ymodem_mq,"ydm",&ymodem_qp[0],200,sizeof(ymodem_qp),RT_IPC_FLAG_FIFO);
    dev.USARTx=ota.port;
    dev.mq=&ymodem_mq;
//    UARTx_Init(USART1,&USART_4G_S,115200,&ymodem_mq);
//    UART1_Init(115200);
    enable_output_log = 1;
    rt_kprintf("Save firmware on \"%s\" partition with device \"uart1\".\n", recv_partition);
    rt_kprintf("Warning: Ymodem has started! This operator will not recovery.\n");
    rt_kprintf("Please select the ota firmware file and use Ymodem to send.\n");

    if (!rym_recv_on_device(&rctx, &dev, ymodem_on_begin, ymodem_on_data, NULL, RT_TICK_PER_SECOND))
    {
        rt_kprintf("Download firmware to flash success.\n");
        rt_kprintf("System now will restart...\r\n");
        
        /* wait some time for terminal response finish */
        rt_thread_delay(rt_tick_from_millisecond(200));

        /* Reset the device, Start new firmware */
        extern void rt_hw_cpu_reset(void);
        rt_hw_cpu_reset();
        /* wait some time for terminal response finish */
        rt_thread_delay(rt_tick_from_millisecond(200));
    }
    else
    {
        /* wait some time for terminal response finish */
        rt_thread_delay(RT_TICK_PER_SECOND);
        rt_kprintf("Update firmware fail.\n");
    }

    return;
}
void ymodem_init(void)
{
    rt_err_t result;
    result = rt_thread_init(&ymd_th,"ydm",ymodem,RT_NULL,&stack_ymd[0],sizeof(stack_ymd), 5, 50);//25优先级，时间片50ms
    if (result != RT_EOK)
    {
        rt_kprintf("init thread app failed.\n");
    }
}

void ymodem_start(void)
{

		rt_kprintf("mcu ota start!\r\n");
		ota.ota_flag = 1;
    rt_thread_startup(&ymd_th);
}

void ymodem(void *param)
{
    ymodem_ota(RT_NULL,RT_NULL);
}

/**
 * Function    ota_app_vtor_reconfig
 * Description Set Vector Table base location to the start addr of app(RT_APP_PART_ADDR).
*/
static int ota_app_vtor_reconfig(void)
{
    #define NVIC_VTOR_MASK   0x3FFFFF80
    /* Set the Vector Table base location by user application firmware definition */
    SCB->VTOR = RT_APP_PART_ADDR & NVIC_VTOR_MASK;

    return 0;
}
INIT_BOARD_EXPORT(ota_app_vtor_reconfig);
/**
 * msh />ymodem_ota
*/
//MSH_CMD_EXPORT(ymodem_ota, Use Y-MODEM to download the firmware);

#endif /* PKG_USING_YMODEM_OTA */
