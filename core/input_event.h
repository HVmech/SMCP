#pragma once
#ifndef SMCP_INPUT_EVENT_H
#define SMCP_INPUT_EVENT_H

#include <core/app_context.h>
#include <core/input_state.h>
#include <core/event_bus.h>
#include <core/event.h>

#include <services/LCD_service.h>

#include <drivers/time_driver.h>

#include <common/asm.h>

#define CONST_INPUT_STATE_UPDATE_INTERVAL 1000

static inline bool generate_input_update_event(event_bus_t *bus) {
    static bool error_prev = false;
    static uint32_t error_start_time = 0;

    if (app_context.input_context.error == true) {
        if (error_prev == false) {
            error_start_time = get_current_time_ms();
        }

        if (get_current_time_ms() - error_start_time >= CONST_INPUT_STATE_UPDATE_INTERVAL) {
            app_context.input_context.error = false;

            event_t evt = {
                .id = EVENT_INPUT_ERROR_DISCARD,
                .priority = EVENT_PRIORITY_NORMAL,
                .flags = EVENT_FLAG_NONE,
                .timestamp = get_current_time_ms()
            };

            event_bus_post_from_isr(bus, &evt);
            MACRO_ASM_DATA_SYNC_BARRIER;
        }
    }

    error_prev = app_context.input_context.error;
    return app_context.input_context.error;
}

#endif // SMCP_INPUT_EVENT_H