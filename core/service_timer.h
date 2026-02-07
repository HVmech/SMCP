#pragma once

#ifndef SMCP_SERVICE_TIMER_H
#define SMCP_SERVICE_TIMER_H

#include <common/types.h>

void service_timer_init(uint32_t period_ms); // Настройка периода (в мс)
void service_timer_enable(void); // Запуск таймера
void service_timer_disable(void); // Остановка таймера

#endif // SMCP_SERVICE_TIMER_H
