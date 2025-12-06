#include <drivers/smcp_gp_timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

static uint32_t timer_to_rcc(uint32_t base) {
    if (base == TIM2) return RCC_TIM2;
    if (base == TIM3) return RCC_TIM3;
    if (base == TIM4) return RCC_TIM4;
    return 0;
}

void smcp_gp_timer_init(const smcp_gp_timer_t *t, uint32_t freq_hz) {
    uint32_t rcc = timer_to_rcc(t->base);
    if (rcc) rcc_periph_clock_enable(rcc);

    // APB1 = 8 МГц (HSI), TIMx = 8 МГц
    uint32_t prescaler = (8000000 / 10000) - 1;  // 10 кГц тактовая
    uint32_t period = (10000 / freq_hz) - 1;

    timer_set_prescaler(t->base, prescaler);
    timer_set_period(t->base, period);
    timer_enable_irq(t->base, TIM_DIER_UIE);
    timer_continuous_mode(t->base);
}

void smcp_gp_timer_start(const smcp_gp_timer_t *t) {
    timer_enable_counter(t->base);
    nvic_enable_irq(t->irq);
}

void smcp_gp_timer_stop(const smcp_gp_timer_t *t) {
    timer_disable_counter(t->base);
    nvic_disable_irq(t->irq);
}

void smcp_gp_timer_clear_irq(const smcp_gp_timer_t *t) {
    timer_clear_flag(t->base, TIM_SR_UIF);
}