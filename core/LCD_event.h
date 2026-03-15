#pragma once
#ifndef SMCP_LCD_EVENT_H
#define SMCP_LCD_EVENT_H

#include <globals/LCD_globals.h>

#include <services/LCD_service.h>

#include <drivers/time_driver.h>

static inline bool generate_LCD_update_event(void) {
    static uint32_t last_blink_update = 0;
    bool result = true;

    if (get_current_time_ms() - last_blink_update >= CONST_LCD_BLINK_INTERVAL) {
        last_blink_update = get_current_time_ms();

        if (g_lcd_blink) {
            g_lcd_blink_phase = !g_lcd_blink_phase;
            LCD_update_request(true);
        }
        else { result = false; }
    }
    return result;
}

#endif // SMCP_LCD_EVENT_H