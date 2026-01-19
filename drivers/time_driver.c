#include <drivers/SysTick_driver.h>
#include <drivers/time_driver.h>

inline uint32_t get_current_time_ms(void) {
    return get_SysTick(); // Получаем значение SysTick
}

inline uint32_t get_elapsed_time_ms(uint32_t start_time_ms) {
    uint32_t current_time_ms = get_SysTick(); // Получаем значение SysTick
    
    if (current_time_ms < start_time_ms) { // Проверка переполнения
        return (0xFFFFFFFF - start_time_ms) + current_time_ms + 1;
    }
    return current_time_ms - start_time_ms;
}