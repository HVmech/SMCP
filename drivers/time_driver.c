#include <drivers/time_driver.h>

void delay_ms(uint32_t delay_time_ms) {
    DEBUG_ASSERT(delay_time_ms < (uint32_t)-1);
    const uint32_t current_time_ms = get_current_time_ms();
    while(get_elapsed_time_ms(current_time_ms, get_current_time_ms()) < delay_time_ms)  { MACRO_ASM_DO_NOTHING; }
}