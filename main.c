
#include <core/state_manager.h>

#include <core/event_dispatcher.h>
#include <core/event_bus.h>
#include <core/event.h>
#include <core/service_timer.h>

#include <services/debug_serial_service.h>
#include <services/LED_service.h>
#include <services/motion_controller_service.h>
#include <services/LCD_service.h>

#include <drivers/matrix_keyboard_driver.h>
#include <drivers/RTC_driver.h>
#include <drivers/SysTick_driver.h>
#include <drivers/time_driver.h>

#include <common/ring_buffer.h>
#include <common/asm.h>
#include <common/board.h>

#include <libopencm3/cm3/nvic.h>

#define EVENT_QUEUE_HIGH_SIZE   8
#define EVENT_QUEUE_NORMAL_SIZE 8
#define EVENT_QUEUE_LOW_SIZE    8

event_bus_t g_event_bus;

static event_t event_queue_high_storage[EVENT_QUEUE_HIGH_SIZE];
static event_t event_queue_normal_storage[EVENT_QUEUE_NORMAL_SIZE];
static event_t event_queue_low_storage[EVENT_QUEUE_LOW_SIZE];

static ring_buffer_t event_queue_high;
static ring_buffer_t event_queue_low;
static ring_buffer_t event_queue_normal;

/* void motor_test_helper_handler(const event_t *evt_inp) {
    if (evt_inp->id != EVENT_MOTOR_ROTATION_PREPARE) { return; }

    event_t evt = {0};
    evt.id = EVENT_MOTOR_PREPARATION_COMPLETE;
    evt.priority = EVENT_PRIORITY_HIGH;
    evt.flags = EVENT_FLAG_NONE;
    evt.timestamp = g_SysTick_cnt;

    event_bus_t *bus = event_dispatcher_get_bus();
    event_bus_post(bus, &evt);
} */

void keyboard_test_helper_handler(const event_t *evt_inp) {
    switch (evt_inp->id) {
        case EVENT_KEY_PRESS: {
            debug_serial_printf("KEY PRESS %u", evt_inp->payload.data.unsigned_value);
            break;
        }
        case EVENT_KEY_RELEASE: {
            const uint16_t key = evt_inp->payload.data.unsigned_value & ((1 << 8) - 1);
            const uint32_t duration = evt_inp->payload.data.unsigned_value >> 8;
            debug_serial_printf("KEY RELEASE %u[%ums]", key, duration);
            break;
        }
        case EVENT_KEY_REPEAT: {
            debug_serial_printf("KEY REPEAT %u", evt_inp->payload.data.unsigned_value);
            break;
        }
        default: {
            break;
        }
    }
}

int main(void)
{
    RTC_init();
    SysTick_init();

    ring_buffer_t *event_queues[EVENT_PRIORITY_NUM] = {
        &event_queue_high,
        &event_queue_normal,
        &event_queue_low
    };

    matrix_keyboard_config_t keyboard_config = {
        .column_pins = {PA1, PA2, PA3, PA4},
        .row_pins = {PB1, PB0, PA7, PA6, PA5}
    };

    ring_buffer_init(&event_queue_high, event_queue_high_storage, EVENT_QUEUE_HIGH_SIZE, sizeof(event_t));
    ring_buffer_init(&event_queue_normal, event_queue_normal_storage, EVENT_QUEUE_NORMAL_SIZE, sizeof(event_t));
    ring_buffer_init(&event_queue_low, event_queue_low_storage, EVENT_QUEUE_LOW_SIZE, sizeof(event_t));

    event_bus_init(&g_event_bus, event_queues);
    event_dispatcher_init(&g_event_bus);

    service_timer_init(20); // базовый период 20 мс
    if (!debug_serial_init(USART_1, 115200, true, true)) { return -1; }
    LED_service_init_led(LED_BUILTIN, PC13, true);
    LED_service_init_led(LED_MOTOR_STOP_BUTTON, PB13, false);
    motion_controller_init(PA9, PA10);
    matrix_keyboard_init(&keyboard_config);

    //event_bus_subscribe(&g_event_bus, EVENT_LED_SERVICE_UPDATE, LED_service_handle_event);
    event_bus_subscribe(&g_event_bus, EVENT_LED_CONTROL, LED_service_handle_event);
    event_bus_subscribe(&g_event_bus, EVENT_USART1_RX, debug_serial_handle_event);
    //event_bus_subscribe(&g_event_bus, EVENT_MOTOR_ROTATION_PREPARE, motor_test_helper_handler);

    event_bus_subscribe(&g_event_bus, EVENT_KEY_PRESS, keyboard_test_helper_handler);
    event_bus_subscribe(&g_event_bus, EVENT_KEY_RELEASE, keyboard_test_helper_handler);
    event_bus_subscribe(&g_event_bus, EVENT_KEY_REPEAT, keyboard_test_helper_handler);

    event_bus_subscribe(&g_event_bus, EVENT_LCD_UPDATE_REQUEST, LCD_test_helper_handler);

    state_manager_init();

#ifdef DEBUG
    debug_serial_printf("[%u] DEBUG configuration started\r\n", get_current_time_ms());
#endif

    debug_serial_printf("[%u] System initialized\r\n", get_current_time_ms()); delay_ms(1000); // Отладка

    while (1) {
        if (event_dispatcher_process()) {
            // nothing
        }
        else {
            MACRO_ASM_DO_NOTHING;
        }
    }

    return 0;
}