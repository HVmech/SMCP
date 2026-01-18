#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

void temp_c() {
    uint32_t rcc = RCC_TIM2;
    if (rcc) rcc_periph_clock_enable(rcc);

    uint32_t prescaler = (8000000 / 10000) - 1;
    uint32_t period = (10000 / 10) - 1;

    timer_set_prescaler(TIM2, prescaler);
    timer_set_period(TIM2, period);
    timer_enable_irq(TIM2, TIM_DIER_UIE);
    timer_continuous_mode(TIM2);
    timer_enable_counter(TIM2);
    nvic_enable_irq(NVIC_TIM2_IRQ);
}

void temp_d() {
    timer_clear_flag(TIM2, TIM_SR_UIF);
}