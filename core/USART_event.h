#pragma once

#ifndef SMCP_USART_EVENT_H
#define SMCP_USART_EVENT_H

#include <globals/USART_globals.h>

#include <core/event_bus.h>
#include <core/event.h>

#include <drivers/SysTick_driver.h>

#include <common/asm.h>

#define CONST_USART_RX_UPDATE_INTERVAL 20

static inline bool generate_USART_update_event(event_bus_t *bus) {
    static uint32_t last_upd_tick = 0;
    bool result = true;

    if (g_SysTick_cnt - last_upd_tick >= CONST_USART_RX_UPDATE_INTERVAL) {
        last_upd_tick = g_SysTick_cnt;

        result = false;
        for (uint8_t i = 0; i < USART_CNT; ++i) {
            if (g_usart_rx_pending[i]) {
                event_t evt = {
                    .id = EVENT_USART1_RX + i,
                    .priority = EVENT_PRIORITY_NORMAL,
                    .flags = EVENT_FLAG_NONE,
                    .timestamp = g_SysTick_cnt
                };

                event_bus_post_from_isr(bus, &evt);
                result = true;
                MACRO_ASM_DATA_SYNC_BARRIER;
            }
        }
    }

    return result;
}

#endif // SMCP_USART_EVENT_H