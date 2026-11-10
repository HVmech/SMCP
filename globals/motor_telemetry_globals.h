#pragma once
#ifndef SMCP_MOTOR_TELEMETRY_GLOBALS_H
#define SMCP_MOTOR_TELEMETRY_GLOBALS_H

#include <common/types.h>

typedef struct { // Структура телеметрии движения
    uint8_t active_phase; // Текущая фаза движения
    uint32_t progress_percentage; // Общий прогресс в процентах
} motor_telemetry_t;

extern volatile bool g_generate_motor_telemetry_updates; // Признак включения телеметрии
extern volatile bool g_motor_telemetry_started; // Признак запуска телеметрии
extern volatile motor_telemetry_t g_motor_telemetry; // Переменная для хранения телеметрии

#endif // SMCP_MOTOR_TELEMETRY_GLOBALS_H