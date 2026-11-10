#include <core/state_manager.h>
#include <core/event_dispatcher.h>
#include <core/event_bus.h>
#include <core/event.h>
#include <core/service_timer.h>
#include <services/debug_serial_service.h>
#include <services/LED_patterns.h>
#include <services/LED_service.h>
#include <services/motion_controller_service.h>
#include <services/LCD_service.h>
#include <drivers/matrix_keyboard_driver.h>
#include <drivers/RTC_driver.h>
#include <drivers/SysTick_driver.h>
#include <drivers/time_driver.h>
#include <drivers/LCD_driver.h>
#include <common/ring_buffer.h>
#include <common/asm.h>
#include <common/board.h>
#include <libopencm3/cm3/nvic.h>

// TODO: config?
#define EVENT_QUEUE_HIGH_SIZE   8
#define EVENT_QUEUE_NORMAL_SIZE 8
#define EVENT_QUEUE_LOW_SIZE    8

// TODO: transfer?
event_bus_t g_event_bus;

static event_t event_queue_high_storage[EVENT_QUEUE_HIGH_SIZE];
static event_t event_queue_normal_storage[EVENT_QUEUE_NORMAL_SIZE];
static event_t event_queue_low_storage[EVENT_QUEUE_LOW_SIZE];

static ring_buffer_t event_queue_high;
static ring_buffer_t event_queue_low;
static ring_buffer_t event_queue_normal;

// TODO: transfer?
void keyboard_test_helper_handler(const event_t *evt_inp) {
    switch (evt_inp->id) {
        case EVENT_KEY_PRESS: {
            debug_serial_printf("KEY PRESS %u\n", evt_inp->payload.data.unsigned_value);
            break;
        }
        case EVENT_KEY_RELEASE: {
            const uint16_t key = evt_inp->payload.data.unsigned_value & ((1 << 8) - 1);
            const uint32_t duration = evt_inp->payload.data.unsigned_value >> 8;
            debug_serial_printf("KEY RELEASE %u : %ums\n", key, duration);
            break;
        }
        case EVENT_KEY_REPEAT: {
            debug_serial_printf("KEY REPEAT %u\n", evt_inp->payload.data.unsigned_value);
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

    // TODO: Transfer to other module?
    ring_buffer_t *event_queues[EVENT_PRIORITY_NUM] = {
        &event_queue_high,
        &event_queue_normal,
        &event_queue_low
    };

    matrix_keyboard_config_t keyboard_config = {
        .column_pins = {PA4, PA3, PA2, PA1},
        .row_pins = {PB1, PB0, PA7, PA6, PA5}
    };

    ring_buffer_init(&event_queue_high, event_queue_high_storage, EVENT_QUEUE_HIGH_SIZE, sizeof(event_t));
    ring_buffer_init(&event_queue_normal, event_queue_normal_storage, EVENT_QUEUE_NORMAL_SIZE, sizeof(event_t));
    ring_buffer_init(&event_queue_low, event_queue_low_storage, EVENT_QUEUE_LOW_SIZE, sizeof(event_t));

    event_bus_init(&g_event_bus, event_queues);
    event_dispatcher_init(&g_event_bus);

    // TODO: transfer to other module?
    service_timer_init(20); // базовый период 20 мс

    // TODO: why?
    if (!debug_serial_init(USART_1, 115200, true, true)) { return -1; }

    // TODO: transfer?
    LED_service_init_led(LED_BUILTIN, PC13, true);
    LED_service_init_led(LED_MOTOR_STOP_BUTTON, PB13, true);
    LED_service_init_led(BUZZER_CONTROL, PB14, false);

    // TODO: Transfer pins to board module?
    motion_controller_init(PA9, PA10);

    // TODO: 
    matrix_keyboard_init(&keyboard_config);

    LCD_init();

    // TODO: transfer?
    event_bus_subscribe(&g_event_bus, EVENT_LED_SERVICE_UPDATE, LED_service_handle_event);
    event_bus_subscribe(&g_event_bus, EVENT_LED_CONTROL, LED_service_handle_event);
    event_bus_subscribe(&g_event_bus, EVENT_USART1_RX, debug_serial_handle_event);
    event_bus_subscribe(&g_event_bus, EVENT_KEY_PRESS, keyboard_test_helper_handler);
    event_bus_subscribe(&g_event_bus, EVENT_KEY_RELEASE, keyboard_test_helper_handler);
    event_bus_subscribe(&g_event_bus, EVENT_KEY_REPEAT, keyboard_test_helper_handler);
    event_bus_subscribe(&g_event_bus, EVENT_MOTOR_ROTATION_COMPLETE, LED_service_handle_event);

    state_manager_init();

    // TODO: need debug?
#ifdef DEBUG
    debug_serial_printf("[%u] DEBUG configuration started\r\n", get_current_time_ms());
#endif

    debug_serial_printf("[%u] System initialized\r\n", get_current_time_ms()); delay_ms(1000); // Отладка

    // TODO: make event?
    LED_service_execute(BUZZER_CONTROL, &LED_pattern_heartbeat, false);

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