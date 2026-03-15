#pragma once


#ifndef SMCP_INPUT_EVENT_H
#define SMCP_INPUT_EVENT_H

#include <core/app_context.h>

#include <services/LCD_service.h>

#include <drivers/time_driver.h>

#include <common/asm.h>

#define CONST_INPUT_STATE_UPDATE_INTERVAL 1000
#define CONST_ACTIVE_INPUT_ROW 1
#define CONST_ACTIVE_INPUT_COLUMN 15

static inline bool generate_input_update_event(void) {
    static bool error_prev = false;
    static uint32_t error_start_time = 0;

    if (app_context.input_context.data.error == true) {
        if (error_prev == false) {
            error_start_time = get_current_time_ms();

            LCD_clear_line(CONST_ACTIVE_INPUT_ROW);
            //LCD_display_input_angle(CONST_ACTIVE_INPUT_ROW, CONST_ACTIVE_INPUT_COLUMN, true);
        }

        if (get_current_time_ms() - error_start_time >= CONST_INPUT_STATE_UPDATE_INTERVAL) {
            app_context.input_context.data.error = false;

            LCD_clear_line(CONST_ACTIVE_INPUT_ROW);
            LCD_set_string(CONST_ACTIVE_INPUT_ROW, 0, "INCORRECT INPUT!", true, false);

            LCD_update_request(true);
            MACRO_ASM_DATA_SYNC_BARRIER;
        }
    }

    error_prev = app_context.input_context.data.error;
    return app_context.input_context.data.error;
}

#endif // SMCP_INPUT_EVENT_H