#pragma once

#ifndef SMCP_DEBOUNCE_TIMER_DRIVER_H
#define SMCP_DEBOUNCE_TIMER_DRIVER_H

#include <common/types.h>

void debounce_timer_init(uint32_t period_ms, uint8_t priority, void_func_ptr_t ISR_handler_hook); // Настройка периода (в мс)
void debounce_timer_enable(void); // Запуск таймера
void debounce_timer_disable(void); // Остановка таймера

#endif // SMCP_DEBOUNCE_TIMER_DRIVER_H
