#pragma once

#ifndef SMCP_LED_EVENT_H
#define SMCP_LED_EVENT_H

#include <globals/LED_globals.h>

#include <core/event_bus.h>
#include <core/event.h>

#include <drivers/SysTick_driver.h>

#include <common/asm.h>

#define CONST_LED_SERVICE_UPDATE_INTERVAL 20

static inline bool generate_LED_update_event(event_bus_t *bus) {
    static uint32_t last_upd_tick = 0;
    bool result = true;

    if (g_SysTick_cnt - last_upd_tick >= CONST_LED_SERVICE_UPDATE_INTERVAL) {
        last_upd_tick = g_SysTick_cnt;
        if (g_generate_led_updates) {
            event_t evt = {
                .id = EVENT_LED_SERVICE_UPDATE,
                .priority = EVENT_PRIORITY_LOW,
                .flags = EVENT_FLAG_NONE,
                .timestamp = g_SysTick_cnt
            };

            event_bus_post_from_isr(bus, &evt);
            MACRO_ASM_DATA_SYNC_BARRIER;
        }
        else { result = false; }
    }
    return result;
}

#endif // SMCP_LED_EVENT_H