#pragma once

#ifndef SMCP_TIME_DRIVER_H
#define SMCP_TIME_DRIVER_H

#include <drivers/SysTick_driver.h>
#include <common/asm.h>
#include <common/types.h>
#include <common/debug_assert.h>

uint32_t get_elapsed_time_ms(uint32_t start_time_ms); // Получение времени в мс до текущего момента

inline uint32_t get_current_time_ms(void) { // Получение счетчика текущего времени в мс
    return SysTick_cnt; // Получаем значение SysTick
}

inline void delay_ms(uint32_t delay_time_ms) {
    DEBUG_ASSERT(delay_time_ms < (uint32_t)-1);
    const uint32_t current_time_ms = get_current_time_ms();
    while(get_elapsed_time_ms(current_time_ms) > delay_time_ms)  { MACRO_ASM_DO_NOTHING; }
}

#endif // SMCP_TIME_DRIVER_H