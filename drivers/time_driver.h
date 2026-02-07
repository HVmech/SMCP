#pragma once

#ifndef SMCP_TIME_DRIVER_H
#define SMCP_TIME_DRIVER_H

#include <drivers/SysTick_driver.h>

#include <common/asm.h>
#include <common/types.h>
#include <common/debug_assert.h>

inline uint32_t get_elapsed_time_ms(uint32_t start_time_ms, uint32_t finish_time_ms) { // Получение времени интервала в мс
    if (finish_time_ms < start_time_ms) { // Проверка переполнения
        return (0xFFFFFFFF - start_time_ms) + finish_time_ms + 1;
    }
    return finish_time_ms - start_time_ms;
}

inline uint32_t get_current_time_ms(void) { return g_SysTick_cnt; } // Получение счетчика текущего времени в мс

void delay_ms(uint32_t delay_time_ms);

#endif // SMCP_TIME_DRIVER_H