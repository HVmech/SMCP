// main.c
#include <services/led_service.h>

int main(void) {
    led_init();
    for (uint32_t i = 0; i < 5000000; ++i) { __asm("nop"); }
    led_blink(10); // 2 Гц → мигание 1 Гц
    for (uint32_t i = 0; i < 5000000; ++i) { __asm("nop"); }
    led_set(true); // 2 Гц → мигание 1 Гц
    for (uint32_t i = 0; i < 5000000; ++i) { __asm("nop"); }
    led_blink(2); // 2 Гц → мигание 1 Гц
    for (uint32_t i = 0; i < 5000000; ++i) { __asm("nop"); }
    led_set(false); // 2 Гц → мигание 1 Гц
    while (1) { __asm("nop"); }
}