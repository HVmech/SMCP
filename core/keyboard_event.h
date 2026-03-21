#pragma once
#ifndef SMCP_KEYBOARD_EVENT_H
#define SMCP_KEYBOARD_EVENT_H

#include <globals/keyboard_globals.h>
#include <drivers/time_driver.h>

static inline bool generate_keyboard_block_event(void) {
    static uint32_t last_update = 0;
    static bool prev_keyboard_block = false;
    bool result = false;

    if (g_keyboard_block) {
        if (!prev_keyboard_block) {
            last_update = get_current_time_ms();
        }

        if (get_current_time_ms() - last_update >= g_keyboard_block_time) {
            g_keyboard_block = false;
            result = false;
        }
        else {
            result = true;
        }

        prev_keyboard_block = g_keyboard_block;
    }

    return result;
}

#endif // SMCP_KEYBOARD_EVENT_H