#include <core/idle_state.h>

#include <core/app_context.h>
#include <core/input_data.h>
#include <core/event_dispatcher.h>
#include <core/event.h>

#include <services/debug_serial_service.h>
#include <services/LCD_service.h>

#include <drivers/matrix_keyboard_driver.h>
#include <drivers/time_driver.h>

#include <common/debug_assert.h>
#include <common/utils.h>
#include <common/types.h>

static inline void idle_state_display_angle(uint8_t row, bool right_alignment) {
    DEBUG_ASSERT(row < LCD_HEIGHT);

    const uint8_t digit_fields = digcnt(app_context.current_angle);
    const uint8_t integer_fields = MAX_VALUE((digit_fields >= INPUT_FRACTIONAL_DIGITS ? digit_fields - INPUT_FRACTIONAL_DIGITS : 0), 1);

    const uint8_t trailing_zeros_count = digit_fields ? trzercnt(app_context.current_angle, INPUT_FRACTIONAL_DIGITS) : INPUT_FRACTIONAL_DIGITS;
    const uint8_t fractional_fields = INPUT_FRACTIONAL_DIGITS - trailing_zeros_count;
    
    const uint8_t dot_fields = fractional_fields ? 1 : 0;
    const uint8_t measurement_fields = 1;

    const uint8_t all_angle_fields = integer_fields + dot_fields + fractional_fields + measurement_fields;
    DEBUG_ASSERT(all_angle_fields);

    uint32_t angle_value = app_context.current_angle;
    const uint8_t start_pos = right_alignment ? LCD_LENGTH - 1 : all_angle_fields - 1;
    uint8_t pos = start_pos;

    uint8_t digit = 0;

    if (measurement_fields) {
        const unsigned char degree_char = LCD_CHAR_DEGREE; 
        LCD_set_char(row, pos--, degree_char, false);
    }

    for (uint8_t i = 0; i < trailing_zeros_count; ++i) {
        angle_value /= 10;
    }

    if (dot_fields) {
        for (uint8_t i = 0; i < fractional_fields; ++i) {
            digit = angle_value % 10;
            angle_value /= 10;
            LCD_set_integer(row, pos--, digit, false);
        }

        const unsigned char dot_char = LCD_CHAR_DOT; 
        LCD_set_char(row, pos--, dot_char, false);
    }

    for (uint8_t i = 0; i < integer_fields; ++i) {
        digit = angle_value % 10;
        angle_value /= 10;
        LCD_set_integer(row, pos--, digit, false);
    }

    DEBUG_ASSERT(pos == start_pos - all_angle_fields);
}

static inline void idle_state_display(void) {
    LCD_clear_display();
    LCD_set_string(CONST_IDLE_STATE_INFO_ROW, 0, "CURRENT ANGLE:", false);
    idle_state_display_angle(CONST_IDLE_STATE_ANLE_ROW, true);
    LCD_update_request(false);
}

void idle_state_event_handler(const event_t *evt) {
    DEBUG_ASSERT(evt);

    switch (evt->id) {
        case EVENT_KEY_RELEASE: {
            const key_t key = evt->payload.data.unsigned_value & ((1 << 8) - 1);
            const uint32_t duration = evt->payload.data.unsigned_value >> 8;

            if (duration <= NORMAL_PRESS_DURATION_MS) {
                switch (key) {
                    case KEY_F1:
                    case KEY_F2: {
                        const uint8_t input_mode = key - KEY_F1;
                        app_context.input_context.mode = input_mode;
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

static inline void idle_state_manage_subscriptions(bool state) {
    event_bus_t *bus = event_dispatcher_get_bus();

    if (state) {
        event_bus_subscribe(bus, EVENT_KEY_RELEASE, idle_state_event_handler);
    }
    else {
        event_bus_unsubscribe(bus, EVENT_KEY_RELEASE, idle_state_event_handler);
    }
}

void idle_state_enter(void) {
    idle_state_display();
    idle_state_manage_subscriptions(true);
    debug_serial_printf("IDLE\n");
}

void idle_state_exit(void) {
    idle_state_manage_subscriptions(false);
    debug_serial_printf("STATE: IDLE --> ");
}

const app_state_t idle_state = {
    .enter = idle_state_enter,
    .exit = idle_state_exit,
    .event_handler = idle_state_event_handler
};