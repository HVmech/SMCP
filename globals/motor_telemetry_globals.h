#pragma once
#ifndef SMCP_MOTOR_TELEMETRY_GLOBALS_H
#define SMCP_MOTOR_TELEMETRY_GLOBALS_H

#include <common/types.h>

typedef struct {
    uint32_t updates_generated;
    uint8_t active_phase;
} motor_telemetry_t;

extern volatile bool g_generate_motor_telemetry_updates;
extern volatile uint8_t g_current_phase;
extern volatile uint32_t g_phase_progress_percentage;


#endif // SMCP_MOTOR_TELEMETRY_GLOBALS_H