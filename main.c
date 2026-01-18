// main.c
#include <temp.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

int main(void) {
    temp_c();
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    while (1) { __asm("nop"); }
}

void TIM2_Handler() {
    temp_d();
    gpio_toggle(GPIOC, GPIO13);
}
