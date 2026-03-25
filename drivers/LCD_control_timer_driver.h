#pragma once

#ifndef SMCP_LCD_CONTROL_TIMER_DRIVER_H
#define SMCP_LCD_CONTROL_TIMER_DRIVER_H

#include <common/types.h>

//#define LCD_PROCESS_FREQUENCY_MS 1
#define LCD_TIMEOUT_TIME_MS 10

void LCD_control_timer_init(uint32_t period_ms, uint8_t priority, int_func_ptr_t ISR_handler_hook, void_func_ptr_t ISR_delay_hook); // Настройка периода (в мс)
bool LCD_control_timer_is_running(void);
void LCD_control_timer_enable(void); // Запуск таймера
void LCD_control_timer_disable(void); // Остановка таймера
void LCD_control_timer_async_delay_us(uint32_t delay_ms);

#endif // SMCP_LCD_CONTROL_TIMER_DRIVER_H
