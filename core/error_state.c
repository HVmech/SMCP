#include <core/error_state.h>

void error_state_enter(void) {

}

void error_state_exit(void) {

}

void error_state_event_handler(const event_t *evt) {
    (void)evt;
}

const app_state_t error_state = {
    .enter = error_state_enter,
    .exit = error_state_exit,
    .event_handler = error_state_event_handler
};