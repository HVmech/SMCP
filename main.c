#include <core/event_bus.h>
#include <core/event_dispatcher.h>
#include <core/service_timer.h>

#include <services/debug_serial_service.h>
#include <services/LED_service.h>

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

int main(void)
{
    RTC_init();
    SysTick_init();

    ring_buffer_t *event_queues[EVENT_PRIORITY_NUM] = {
        &event_queue_high,
        &event_queue_normal,
        &event_queue_low
    };

    ring_buffer_init(&event_queue_high, event_queue_high_storage, EVENT_QUEUE_HIGH_SIZE, sizeof(event_t));
    ring_buffer_init(&event_queue_normal, event_queue_normal_storage, EVENT_QUEUE_NORMAL_SIZE, sizeof(event_t));
    ring_buffer_init(&event_queue_low, event_queue_low_storage, EVENT_QUEUE_LOW_SIZE, sizeof(event_t));

    event_bus_init(&g_event_bus, event_queues);
    event_dispatcher_init(&g_event_bus);

    service_timer_init(20);   // базовый период 20 мс

    if (!debug_serial_init(USART_1, 115200, false, true)) { return -1; }

    if (!LED_service_init_led(0, PC13, true)) { return -1; }

    event_bus_subscribe(&g_event_bus, EVENT_LED_SERVICE_UPDATE, LED_service_handle_event);

    event_bus_subscribe(&g_event_bus, EVENT_LED_CONTROL, LED_service_handle_event);

    event_bus_subscribe(&g_event_bus, EVENT_USART1_RX, debug_serial_handle_event);

#ifdef DEBUG
    debug_serial_printf("[%u] DEBUG configuration started\r\n", get_current_time_ms());
#endif

    debug_serial_printf("[%u] System initialized\r\n", get_current_time_ms()); delay_ms(1000); // Отладка

    while (1) {
        if (event_dispatcher_process()) {
            debug_serial_printf("[%u] EVENT PROCESSED\n", g_SysTick_cnt);
        }
        else {
            MACRO_ASM_DO_NOTHING;
        }
    }

    return 0;
}