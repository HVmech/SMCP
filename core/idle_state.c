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
    LCD_display_angle(1, 0, app_context.current_angle, false, false);
    LCD_update_request();
}

void idle_state_enter(void) {
    idle_state_display();
    debug_serial_printf("IDLE\n");
}

void idle_state_exit(void) {
    // ...
    debug_serial_printf("STATE: IDLE --> ");
}

void idle_state_event_handler(const event_t *evt) {
    DEBUG_ASSERT(evt);

    switch (evt->id) {
        case EVENT_KEY_RELEASE: {
            const key_t key = evt->payload.data.unsigned_value & ((1 << 8) - 1);
            const uint32_t duration = evt->payload.data.unsigned_value & (~((1 << 8) - 1));

            if (duration <= NORMAL_PRESS_DURATION_MS) {
                switch (key) {
                    case KEY_F1:
                    case KEY_F2: {
                        const uint8_t input_mode = key - KEY_F1;
                        app_context.input_data.mode = input_mode;
                        app_state_transition_request(APP_STATE_INPUT);
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