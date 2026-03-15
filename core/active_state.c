#include <core/active_state.h>

#include <globals/motor_telemetry_globals.h>

#include <core/app_context.h>
#include <core/event_dispatcher.h>
#include <core/event.h>

#include <services/debug_serial_service.h>
#include <services/LCD_service.h>
#include <services/LED_service.h>

#include <drivers/matrix_keyboard_driver.h>
#include <drivers/time_driver.h>

void active_state_enter(void);
void active_state_exit(void);
void active_state_event_handler(const event_t *evt);

static inline void active_state_display_motor_telemetry(uint8_t current_phase, uint32_t phase_progress_percentage) {
    LCD_clear_display();
    LCD_set_string(0, 0, "ROTATING:", false);
    LCD_set_string(1, 0, "Ph: ", false);
    LCD_set_integer(1, 4, current_phase, false);
    LCD_set_string(1, 5, ", Pr: ", false);
    LCD_set_integer(1, 11, phase_progress_percentage, false);
    LCD_set_char(1, 14, '%', false);
}

static inline void active_state_display(void) {
    LCD_set_string(0, 0, "ROTATING:", false);
    active_state_display_motor_telemetry(g_current_phase, g_phase_progress_percentage);
    LCD_update_request(false);
}

static inline void active_state_emergency_button_led_control(bool state) {
    event_t evt = {0};
    event_bus_t *bus = event_dispatcher_get_bus();
    const uint32_t evt_time = get_current_time_ms();

    evt.id = EVENT_LED_CONTROL;
    evt.priority = EVENT_PRIORITY_HIGH;
    evt.flags = EVENT_FLAG_NONE;
    evt.timestamp = evt_time;
    evt.payload.type = EVENT_DATA_UNSIGNED;
    evt.payload.data.unsigned_value = ((uint32_t)LED_MOTOR_STOP_BUTTON << 1) | (state ? 1U : 0U);

    event_bus_post(bus, &evt);
}

static inline void active_state_start_preparation(void) {
    active_state_emergency_button_led_control(true);
}

static inline void active_state_end_preparation_notify(void) {
    event_t evt = {0};
    event_bus_t *bus = event_dispatcher_get_bus();
    const uint32_t evt_time = get_current_time_ms();

    evt.id = EVENT_MOTOR_PREPARATION_COMPLETE;
    evt.priority = EVENT_PRIORITY_NORMAL;
    evt.flags = EVENT_FLAG_NONE;
    evt.timestamp = evt_time;

    event_bus_post(bus, &evt);
}

static inline void active_state_manage_subscriptions(bool state) {
    event_bus_t *bus = event_dispatcher_get_bus();

    if (state) {
        event_bus_subscribe(bus, EVENT_MOTOR_ROTATION_PREPARE, active_state_event_handler);
        event_bus_subscribe(bus, EVENT_MOTOR_TELEMETRY_UPDATE, active_state_event_handler);
        event_bus_subscribe(bus, EVENT_MOTOR_ROTATION_COMPLETE, active_state_event_handler);
        event_bus_subscribe(bus, EVENT_MOTOR_EMERGENCY_STOP, active_state_event_handler);
    }
    else {
        event_bus_unsubscribe(bus, EVENT_MOTOR_ROTATION_PREPARE, active_state_event_handler);
        event_bus_unsubscribe(bus, EVENT_MOTOR_TELEMETRY_UPDATE, active_state_event_handler);
        event_bus_unsubscribe(bus, EVENT_MOTOR_ROTATION_COMPLETE, active_state_event_handler);
        event_bus_unsubscribe(bus, EVENT_MOTOR_EMERGENCY_STOP, active_state_event_handler);
    }
}

void active_state_enter(void) {
    active_state_display();
    active_state_manage_subscriptions(true);

    event_bus_t *bus = event_dispatcher_get_bus();
    const uint32_t evt_time = get_current_time_ms();

    event_t evt = {0};
    evt.id = EVENT_MOTOR_ROTATION_REQUEST;
    evt.priority = EVENT_PRIORITY_NORMAL;
    evt.flags = EVENT_FLAG_NONE;
    evt.timestamp = evt_time;

    evt.payload.type = EVENT_DATA_SIGNED;
    evt.payload.data.signed_value = app_context.input_context.value;
    event_bus_post(bus, &evt);

    debug_serial_printf("ACTIVE\n");
}

void active_state_exit(void) {
    active_state_emergency_button_led_control(false);
    active_state_manage_subscriptions(false);
    debug_serial_printf("STATE: ACTIVE --> ");
}

void active_state_event_handler(const event_t *evt) {
    DEBUG_ASSERT(evt);

    switch (evt->id) {
        case EVENT_MOTOR_ROTATION_PREPARE: {
            active_state_start_preparation();
            active_state_end_preparation_notify();
            break;
        }
        case EVENT_MOTOR_TELEMETRY_UPDATE: {
            active_state_display_motor_telemetry(g_current_phase, g_phase_progress_percentage);
            LCD_update_request(false);
            break;
        }
        case EVENT_MOTOR_ROTATION_COMPLETE: {
            app_state_transition_request(APP_STATE_IDLE);
            break;
        }
        case EVENT_MOTOR_EMERGENCY_STOP: {
            app_state_transition_request(APP_STATE_ERROR);
            break;
        }
        default: { return; }
    }
}

const app_state_t active_state = {
    .enter = active_state_enter,
    .exit = active_state_exit,
    .event_handler = active_state_event_handler
};