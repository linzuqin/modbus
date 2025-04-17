/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rthw.h>
#include <rtconfig.h>
#include "SEGGER_RTT.h"

#ifndef RT_USING_FINSH

#endif

#ifdef RT_USING_FINSH

RT_WEAK char rt_hw_console_getchar(void)
{
    /* Note: the initial value of ch must < 0 */
    int ch = -1;

//#error "TODO 4: Read a char from the uart and assign it to 'ch'."
    if(SEGGER_RTT_HasKey())
        ch=SEGGER_RTT_GetKey();
    return ch;
}

#endif /* RT_USING_FINSH */

