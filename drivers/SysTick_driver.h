#pragma once

#ifndef SMCP_SYSTICK_DRIVER_H
#define SMCP_SYSTICK_DRIVER_H

#include <drivers/RTC_driver.h>

#include <common/types.h>

#define CONST_MS_PER_TICK 1U

extern volatile uint32_t g_SysTick_cnt; // Счетчик тиков

void SysTick_init(void); // Инициализация SysTick

#endif // SMCP_SYSTICK_DRIVER_H