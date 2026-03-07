#include <core/state_manager.h>

#include <core/event_dispatcher.h>
#include <core/event_bus.h>
#include <core/event.h>

#include <services/debug_serial_service.h>

#include <common/debug_assert.h>

app_state_code_t current_state = 0;

void state_manager_init(void) {
    current_state = APP_STATE_IDLE;
    idle_state.enter();
}

void state_manager_event_handler(const event_t *evt) {
    DEBUG_ASSERT(evt);

    if (evt->id == EVENT_STATE_TRANSITION_REQUEST) {
        DEBUG_ASSERT(evt->payload.data.unsigned_value != current_state);

        switch (current_state) {
            case APP_STATE_IDLE: {
                idle_state.exit();
                break;
            }
            case APP_STATE_INPUT: {
                input_state.exit();
                break;
            }
            case APP_STATE_ACTIVE: {
                active_state.exit();
                break;
            }
            case APP_STATE_ERROR: {
                error_state.exit();
                break;
            }
            default: { debug_serial_printf("Unknown current state!"); return; }
        }

        current_state = evt->payload.data.unsigned_value;

        switch (evt->payload.data.unsigned_value) {
            case APP_STATE_IDLE: {
                idle_state.enter();
                break;
            }
            case APP_STATE_INPUT: {
                input_state.enter();
                break;
            }
            case APP_STATE_ACTIVE: {
                active_state.enter();
                break;
            }
            case APP_STATE_ERROR: {
                error_state.enter();
                break;
            }
            default: { debug_serial_printf("Unknown target state!"); return; }
        }
    }
}