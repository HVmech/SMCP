#include <drivers/time_driver.h>

uint32_t get_elapsed_time_ms(uint32_t start_time_ms) {
    const uint32_t current_time_ms = SysTick_cnt; // Получаем значение SysTick
    
    if (current_time_ms < start_time_ms) { // Проверка переполнения
        return (0xFFFFFFFF - start_time_ms) + current_time_ms + 1;
    }
    return current_time_ms - start_time_ms;
}