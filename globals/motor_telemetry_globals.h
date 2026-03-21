#pragma once
#ifndef SMCP_MOTOR_TELEMETRY_GLOBALS_H
#define SMCP_MOTOR_TELEMETRY_GLOBALS_H

#include <common/types.h>

typedef struct {
    uint8_t active_phase;
    uint32_t progress_percentage;
} motor_telemetry_t;

extern volatile bool g_generate_motor_telemetry_updates;
extern volatile motor_telemetry_t g_motor_telemetry;


#endif // SMCP_MOTOR_TELEMETRY_GLOBALS_H