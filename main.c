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

    if (!LED_service_init(&g_BUILTIN_LED, &LED_pattern_heartbeat, PIN_LED_BUILTIN, true, true)) { return -1; }

    LED_service_start(&g_BUILTIN_LED);

    while (1) {
        LED_service_update(&g_BUILTIN_LED);
        __asm("nop");
        if (get_current_time_ms() % 12000 == 11999) {
            if (g_BUILTIN_LED.config->cmd_seq_length > 4) {
                LED_service_execute(&g_BUILTIN_LED, &LED_pattern_blink_500, true);
            } else {
                LED_service_execute(&g_BUILTIN_LED, &LED_pattern_SOS, false);
            }
        }
    }
    return 0;
}


