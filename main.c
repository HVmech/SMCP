#include <services/debug_serial_service.h>
#include <services/LED_patterns.h>
#include <services/LED_service.h>
#include <drivers/time_driver.h>
#include <drivers/RTC_driver.h>
#include <drivers/SysTick_driver.h>
#include <common/board.h>

s_LED_service g_BUILTIN_LED;

int main(void) {
    RTC_init();
    SysTick_init();
    if (!debug_serial_init(USART_1, 115200, false, true)) { return -1; }
    if (!LED_service_init(&g_BUILTIN_LED, &LED_pattern_heartbeat, PIN_LED_BUILTIN, true, true)) { return -1; }

    #ifdef DEBUG
    debug_serial_printf("[%u] DEBUG configuration started\r\n", get_current_time_ms());
    #endif // DEBUG
    debug_serial_printf("[%u] System initialized\r\n", get_current_time_ms());

    LED_service_start(&g_BUILTIN_LED);

    debug_serial_printf("[%u] System started\r\n", get_current_time_ms());

    // Переменные для отслеживания времени
    uint32_t last_switch_time = 0;
    uint32_t last_print_time = 0;
    uint32_t counter = 0;
    bool pattern_toggle = false;  // Флаг для переключения между паттернами

    while (1) {
        LED_service_update(&g_BUILTIN_LED);
        
        uint32_t current_time = get_current_time_ms();
        
        // Переключение паттернов каждые 12 секунд
        if (current_time - last_switch_time >= 12000) {
            last_switch_time = current_time;
            
            if (pattern_toggle) {
                LED_service_execute(&g_BUILTIN_LED, &LED_pattern_blink_500, true);
                debug_serial_printf("[%u] Switched to blink_500\r\n", current_time);
            } else {
                LED_service_execute(&g_BUILTIN_LED, &LED_pattern_SOS, true);
                debug_serial_printf("[%u] Switched to SOS\r\n", current_time);
            }
            
            pattern_toggle = !pattern_toggle;  // Чередование паттернов
        }

        debug_serial_echo_simple();
        
        // Вывод статуса каждую секунду (опционально)
        if (current_time - last_print_time >= 1000) {
            last_print_time = current_time;
            debug_serial_printf("[%u] Uptime: %u sec, Counter: %u\r\n", current_time, current_time / 1000, counter++);
            serial_print_stats();
        }
    }

    return 0;
}