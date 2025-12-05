#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

void TIM1_UP_Handler(void) {
    timer_clear_flag(TIM1, TIM_SR_UIF);
    gpio_toggle(GPIOC, GPIO13);
}

int main(void) {
    // Тактирование
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_TIM1);

    // GPIO
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    // Таймер: 1 кГц → 1 Гц с ARR=999
    timer_set_prescaler(TIM1, 7199);  // 72 MHz / 7200 = 10 kHz
    timer_set_period(TIM1, 999);      // 1000 тактов → 1 Гц
    timer_enable_irq(TIM1, TIM_DIER_UIE);
    timer_continuous_mode(TIM1);
    timer_enable_counter(TIM1);

    // NVIC
    nvic_enable_irq(NVIC_TIM1_UP_IRQ);  // ← libopencm3 знает: 25

    while (1) {
        __asm("nop");
    }
}