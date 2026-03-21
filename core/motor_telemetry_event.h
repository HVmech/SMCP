#pragma once

#ifndef SMCP_MOTOR_TELEMETRY_EVENT_H
#define SMCP_MOTOR_TELEMETRY_EVENT_H

#include <globals/motor_telemetry_globals.h>

#include <core/event_bus.h>
#include <core/event.h>

#include <services/motion_execution_service.h>

#include <drivers/SysTick_driver.h>

#include <common/asm.h>

#define CONST_MOTOR_TELEMETRY_UPDATE_INTERVAL 500

static inline bool generate_motor_telemetry_update_event(event_bus_t *bus) {
    static uint32_t last_upd_tick = 0;
    bool result = true;

    if (g_SysTick_cnt - last_upd_tick >= CONST_MOTOR_TELEMETRY_UPDATE_INTERVAL) {
        motion_executor_telemetry_update();

        last_upd_tick = g_SysTick_cnt;
        if (g_generate_motor_telemetry_updates) {
            event_t evt = {
                .id = EVENT_MOTOR_TELEMETRY_UPDATE,
                .priority = EVENT_PRIORITY_LOW,
                .flags = EVENT_FLAG_NONE,
                .timestamp = g_SysTick_cnt
            };

            event_bus_post_from_isr(bus, &evt);
            MACRO_ASM_DATA_SYNC_BARRIER;
        }
        else {
            result = false;
            g_motor_telemetry.active_phase = 0;
            g_motor_telemetry.progress_percentage = 0;
        }
    }
    return result;
}

#endif // SMCP_MOTOR_TELEMETRY_EVENT_H