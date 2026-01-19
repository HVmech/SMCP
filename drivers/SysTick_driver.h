#pragma once

#ifndef SMCP_SYSTICK_DRIVER_H
#define SMCP_SYSTICK_DRIVER_H

#include <common/types.h>
#include <drivers/RTC_driver.h>

#define CONST_MS_PER_TICK 1U

void SysTick_init(void); // Инициализация SysTick

uint32_t get_SysTick(void); // Получение текущего тика

#endif // SMCP_SYSTICK_DRIVER_H