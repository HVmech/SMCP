// drivers/smcp_gp_timer.c
#include <drivers/smcp_gp_timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

// Статические callback’и для TIM2/3/4
static smcp_timer_callback_t tim2_cb = 0;
static smcp_timer_callback_t tim3_cb = 0;
static smcp_timer_callback_t tim4_cb = 0;

static uint32_t timer_to_rcc(uint32_t base) {
    if (base == TIM2) return RCC_TIM2;
    if (base == TIM3) return RCC_TIM3;
    if (base == TIM4) return RCC_TIM4;
    return 0;
}

void smcp_gp_timer_init(const smcp_gp_timer_t *t, uint32_t freq_hz) {
    uint32_t rcc = timer_to_rcc(t->base);
    if (rcc) rcc_periph_clock_enable(rcc);

    uint32_t prescaler = (8000000 / 10000) - 1;
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

void smcp_gp_timer_set_callback(const smcp_gp_timer_t *t, smcp_timer_callback_t cb) {
    if (t->base == TIM2) {
        tim2_cb = cb;
    } else if (t->base == TIM3) {
        tim3_cb = cb;
    } else if (t->base == TIM4) {
        tim4_cb = cb;
    }
}

// Обработчики — глобальные, как в startup.s
void TIM2_Handler(void) {
    if (tim2_cb) tim2_cb();
    smcp_gp_timer_clear_irq(&(smcp_gp_timer_t){TIM2, NVIC_TIM2_IRQ});
}

void TIM3_Handler(void) {
    if (tim3_cb) tim3_cb();
    smcp_gp_timer_clear_irq(&(smcp_gp_timer_t){TIM3, NVIC_TIM3_IRQ});
}

void TIM4_Handler(void) {
    if (tim4_cb) tim4_cb();
    smcp_gp_timer_clear_irq(&(smcp_gp_timer_t){TIM4, NVIC_TIM4_IRQ});
}