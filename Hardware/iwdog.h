#ifndef __IWDOG_H
#define __IWDOG_H
#include "stm32f10x.h"
#include "rtthread.h"
#include "log.h"

// 初始化独立看门狗
/**
 * @file iwdog.c
 * @brief Independent Watchdog (IWDG) initialization and feed functions.
 *
 * This file contains functions to initialize and feed the Independent Watchdog
 * (IWDG) timer. The IWDG is used to reset the system in case of software 
 * malfunctions or prolonged inactivity.
 */

/**
 * @brief Initializes the Independent Watchdog (IWDG).
 *
 * This function configures the IWDG with the specified prescaler and reload
 * value. It enables write access, sets the prescaler, sets the reload value,
 * reloads the counter, and starts the watchdog timer.
 *
 * @param prescaler The prescaler value for the IWDG clock. This determines the
 *                  division factor for the IWDG clock.
 * @param reload    The reload value for the IWDG counter. This determines the
 *                  timeout period before the watchdog resets the system.
 */
void IWDG_Init(uint8_t prescaler, uint16_t reload);

/**
 * @brief Feeds the Independent Watchdog (IWDG).
 *
 * This function reloads the IWDG counter to prevent the watchdog from resetting
 * the system. It should be called periodically within the timeout period to
 * ensure normal operation.
 */
void IWDG_Feed(void);

#endif /* __IWDOG_H */