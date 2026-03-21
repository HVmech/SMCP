#include <core/app_state.h>

#include <globals/keyboard_globals.h>

#include <core/event_dispatcher.h>
#include <core/event_bus.h>
#include <core/event.h>
#include <core/service_timer.h>

#include <drivers/time_driver.h>

void app_state_transition_request(app_state_code_t state) {
    event_t evt = {0};
    const uint32_t evt_time = get_current_time_ms();
    event_bus_t *bus = event_dispatcher_get_bus();

    evt.id = EVENT_STATE_TRANSITION_REQUEST;
    evt.priority = EVENT_PRIORITY_NORMAL;
    evt.payload.type = EVENT_DATA_UNSIGNED;
    evt.payload.data.unsigned_value = state;
    evt.timestamp = evt_time;

    event_bus_post(bus, &evt);

    g_keyboard_block = true;
    g_keyboard_block_time = 1000;
    service_timer_enable();
}