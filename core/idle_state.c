#include <core/idle_state.h>

#include <core/app_context.h>
#include <core/event_dispatcher.h>
#include <core/event.h>

#include <services/debug_serial_service.h>
#include <services/LCD_service.h>

#include <drivers/matrix_keyboard_driver.h>
#include <drivers/time_driver.h>

static inline void idle_state_display(void) {
    LCD_set_string(0, 0, "CURRENT ANGLE:", false);
    LCD_display_angle(0, 1, app_context.current_angle);
}

void idle_state_enter(void) {
    event_bus_t *bus = event_dispatcher_get_bus();

    event_t evt = {0};
    evt.id = EVENT_LCD_UPDATE_REQUEST;
    evt.priority = EVENT_PRIORITY_LOW;
    evt.flags = EVENT_FLAG_DEDUPLICATE_LAST;
    evt.timestamp = get_current_time_ms();

    idle_state_display();
    event_bus_post(bus, &evt);

    debug_serial_printf("IDLE\n");
}

void idle_state_exit(void) {
    // ...
    debug_serial_printf("STATE: IDLE --> ");
}

void idle_state_event_handler(const event_t *evt) {
    DEBUG_ASSERT(evt);

    event_bus_t *bus = event_dispatcher_get_bus();

    switch (evt->id) {
        case EVENT_KEY_RELEASE: {
            const key_t key = evt->payload.data.unsigned_value & ((1 << 8) - 1);
            const uint32_t duration = evt->payload.data.unsigned_value & (~((1 << 8) - 1));

            if (duration <= NORMAL_PRESS_DURATION_MS) {
                const uint32_t evt_time = get_current_time_ms();

                switch (key) {
                    case KEY_F1:
                    case KEY_F2: {
                        event_t evt = {0};
                        evt.id = EVENT_STATE_TRANSITION_REQUEST;
                        evt.priority = EVENT_PRIORITY_NORMAL;
                        evt.payload.type = EVENT_DATA_UNSIGNED;

                        const uint8_t input_mode = key - KEY_F1;
                        evt.payload.data.unsigned_value = (input_mode << 8) | APP_STATE_INPUT;
                        evt.timestamp = evt_time;
                        event_bus_post(bus, &evt);
                        break;
                    }
                    default: { return; }
                }
            }
            break;
        }
        default: { return; }
    }
}

const app_state_t idle_state = {
    .enter = idle_state_enter,
    .exit = idle_state_exit,
    .event_handler = idle_state_event_handler
};