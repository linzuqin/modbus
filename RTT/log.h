/**
 * @file log.h
 * @author JohnWay (1973372075@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-19
 * 
 * @copyright Copyright (c) 2024 JohnWay
 * SPDX-License-Identifier: Apache-2.0
 * encoding: utf-8
 * 
 */
 
 #ifndef LOG_H
 #define LOG_H
  
 #include "SEGGER_RTT.h"
  
 #ifdef __cplusplus
 extern "C" {
 #endif
  
 static uint32_t log_cnt = 0;
  
 #define LOG(color, type, fmt, ...) SEGGER_RTT_printf(0, "%s[%d]%s" fmt "%s\r\n", color, log_cnt++, type, ##__VA_ARGS__, RTT_CTRL_RESET)
  
 #define LOG_E(fmt, ...) LOG(RTT_CTRL_TEXT_BRIGHT_RED, "[E]:", fmt, ##__VA_ARGS__)
 #define LOG_W(fmt, ...) LOG(RTT_CTRL_TEXT_BRIGHT_YELLOW, "[W]:", fmt, ##__VA_ARGS__)
 #define LOG_I(fmt, ...) LOG(RTT_CTRL_TEXT_BRIGHT_GREEN, "[I]:", fmt, ##__VA_ARGS__)
  
 #ifdef __cplusplus
 }
 #endif
  
 #endif /* LOG_H */
