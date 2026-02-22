#pragma once
#ifndef SMCP_STEP_TIMER_DRIVER_H
#define SMCP_STEP_TIMER_DRIVER_H

#include <common/types.h>
#include <libopencm3/stm32/timer.h>

typedef struct {
    uint32_t prescaler; // Делитель таймера
    uint32_t arr;       // Период
    uint32_t ccr;       // Ширина импульса
    uint16_t rcr;       // Количество срабатываний для прерывания
} step_timer_config_t;

void step_timer_init(const step_timer_config_t *cfg);
void step_timer_start(void);
void step_timer_stop(void);
void step_timer_set_period(uint32_t arr_value); // Установка периода
void step_timer_set_ccr(uint32_t ccr_value); // Установка ширины импульса
void step_timer_set_rcr(uint16_t rcr_value); // Установка количества срабатываний для прерывания

void step_timer_disable_output(void);

// Включение/отключение прерывания
void step_timer_enable_irq(void);
void step_timer_disable_irq(void);

void step_timer_reset_update_flag(void);
void step_timer_reset_breakup_flag(void);

void step_timer_update_timer(void);

#endif // SMCP_STEP_TIMER_DRIVER_H
