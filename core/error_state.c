#include <core/error_state.h>
#include <core/event_dispatcher.h>
#include <core/event.h>
#include <services/LCD_service.h>
#include <services/debug_serial_service.h>
#include <drivers/matrix_keyboard_driver.h>
#include <drivers/time_driver.h>

void error_state_event_handler(const event_t *evt);
void error_state_enter(void);
void error_state_exit(void);

// Вспомогательные функции:
static inline void error_state_display(void) { // Обновление дисплея с текущим состоянием
    LCD_clear_display();

    LCD_set_string(CONST_ERROR_STATE_INFO_ROW, 0, "ERROR: ", false);
    LCD_set_string(CONST_ERROR_STATE_INFO_ROW, 7, "E-STOP", true);
    LCD_set_string(CONST_ERROR_STATE_INFO_ROW + 1, 0, "ESC FOR RECOVERY", false);

    LCD_update_request(false);
}

static inline void error_state_manage_subscriptions(bool state) { // Управление подписками событий
    event_bus_t *bus = event_dispatcher_get_bus();

    if (state) {
        event_bus_subscribe(bus, EVENT_KEY_RELEASE, error_state_event_handler);
        event_bus_subscribe(bus, EVENT_MOTOR_RECOVERY_COMPLETE, error_state_event_handler);
    }
    else {
        event_bus_unsubscribe(bus, EVENT_KEY_RELEASE, error_state_event_handler);
        event_bus_unsubscribe(bus, EVENT_MOTOR_RECOVERY_COMPLETE, error_state_event_handler);
    }
}

// Основные функции:
void error_state_event_handler(const event_t *evt) { // Обработчик событий
    DEBUG_ASSERT(evt);

    switch (evt->id) {
        case EVENT_KEY_RELEASE: {
            const key_t key = evt->payload.data.unsigned_value & ((1 << 8) - 1);
            const uint32_t duration = evt->payload.data.unsigned_value >> 8;

            if (duration <= LONG_PRESS_DURATION_MS) {
                switch (key) {
                    case KEY_ESC: {
                        event_bus_t *bus = event_dispatcher_get_bus();
                        event_t evt = {0};
                        evt.id = EVENT_MOTOR_RECOVERY_REQUEST;
                        evt.priority = EVENT_PRIORITY_NORMAL;
                        evt.flags = EVENT_FLAG_NONE;
                        evt.timestamp = g_SysTick_cnt;

                        event_bus_post(bus, &evt);
                        break;
                    }
                    default: { return; }
                }
            }
            break;
        }
        case EVENT_MOTOR_RECOVERY_COMPLETE: {
            app_state_transition_request(APP_STATE_IDLE);
            break;
        }
        default: { return; }
    }
}

void error_state_enter(void) { // Инициализация состояния при назначении
    error_state_display();
    error_state_manage_subscriptions(true);
    debug_serial_printf("ERROR\n");
}

void error_state_exit(void) { // Завершение состояния при смене
    error_state_manage_subscriptions(false);
    debug_serial_printf("STATE: ERROR --> ");
}

const app_state_t error_state = { // Структура с указателями на функции состояния
    .enter = error_state_enter,
    .exit = error_state_exit,
    .event_handler = error_state_event_handler
};