#include <drivers/LCD_control_timer_driver.h>

#include <globals/LCD_globals.h>

#include <common/debug_assert.h>
#include <common/types.h>

#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

static volatile int_func_ptr_t LCD_control_timer_ISR_func;
static volatile void_func_ptr_t LCD_control_timer_ISR_delay_end;

static volatile bool LCD_delay_on = false;
static volatile uint32_t LCD_remaining_delay_ms = 0;

void LCD_control_timer_init(uint32_t period_ms, uint8_t priority, int_func_ptr_t ISR_handler_hook, void_func_ptr_t ISR_delay_end_hook) {
    DEBUG_ASSERT(period_ms);
    DEBUG_ASSERT(ISR_handler_hook);
    DEBUG_ASSERT(ISR_delay_end_hook);

    LCD_control_timer_ISR_func = ISR_handler_hook;
    LCD_control_timer_ISR_delay_end = ISR_delay_end_hook;

    rcc_periph_clock_enable(RCC_TIM3);

    timer_set_prescaler(TIM3, 7200 - 1);
    timer_set_period(TIM3, period_ms * 10 - 1);

    timer_update_on_overflow(TIM3);
    timer_enable_irq(TIM3, TIM_DIER_UIE);

    nvic_set_priority(NVIC_TIM3_IRQ, priority);
    nvic_enable_irq(NVIC_TIM3_IRQ);
}

bool LCD_control_timer_is_running() {
    return (TIM_CR1(TIM3) & TIM_CR1_CEN) != 0;
}

void LCD_control_timer_enable(void) { timer_enable_counter(TIM3); }
void LCD_control_timer_disable(void) { timer_disable_counter(TIM3); timer_set_counter(TIM3, 0); }

void LCD_control_timer_async_delay_us(uint32_t delay_ms) {//, void_func_ptr_t callback) {
    //DEBUG_ASSERT(callback);
    DEBUG_ASSERT(delay_ms);
    //DEBUG_ASSERT(!LCD_delay_on);

    if (!LCD_delay_on) {;
        //LCD_control_timer_ISR_delay_end = callback;
        LCD_remaining_delay_ms = delay_ms;
        LCD_delay_on = true;
    }
}

void TIM3_Handler(void) { 
    timer_clear_flag(TIM3, TIM_SR_UIF);

    if (LCD_delay_on) {
        if (!LCD_remaining_delay_ms) {
            LCD_delay_on = false;
            LCD_control_timer_ISR_delay_end();
        }
        --LCD_remaining_delay_ms;
    }
    else {
        if (LCD_control_timer_ISR_func()) {
            ++g_lcd_tick;
        }
        if (g_lcd_tick > LCD_TIMEOUT_TIME_MS) {
            g_lcd_tick = 0;
            LCD_control_timer_disable();
        }
    }
}
