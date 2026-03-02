#include <drivers/debounce_timer_driver.h>

#include <common/debug_assert.h>

#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

static uint32_t debounce_timer_period_ms = 1;
static void_func_ptr_t debounce_timer_ISR_func;

void debounce_timer_init(uint32_t period_ms, uint8_t priority, void_func_ptr_t ISR_handler_hook) {
    DEBUG_ASSERT(period_ms);
    DEBUG_ASSERT(ISR_handler_hook);

    debounce_timer_period_ms = period_ms;
    debounce_timer_ISR_func = ISR_handler_hook;

    rcc_periph_clock_enable(RCC_TIM2);

    timer_set_prescaler(TIM2, 7200 - 1);
    timer_set_period(TIM2, period_ms * 10 - 1);

    timer_update_on_overflow(TIM2);
    timer_enable_irq(TIM2, TIM_DIER_UIE);

    nvic_set_priority(NVIC_TIM2_IRQ, priority);
    nvic_enable_irq(NVIC_TIM2_IRQ);
}

void debounce_timer_enable(void) { timer_enable_counter(TIM2); }
void debounce_timer_disable(void) { timer_disable_counter(TIM2); timer_set_counter(TIM2, 0); }

void TIM2_Handler(void) { 
    timer_clear_flag(TIM2, TIM_SR_UIF);
    debounce_timer_ISR_func();
}
