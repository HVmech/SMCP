#include <core/service_timer.h>

#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

#include <core/event_dispatcher.h>
#include <core/LED_event.h>
#include <core/USART_event.h>

#include <services/debug_serial_service.h>
#include <drivers/time_driver.h>

static uint32_t service_timer_period_ms = 20;

void service_timer_init(uint32_t period_ms)
{
    service_timer_period_ms = period_ms;

    rcc_periph_clock_enable(RCC_TIM4);

    /*
     * Настраиваем TIM4 так, чтобы его период был = period_ms
     * Предполагаем, что SysTick у нас считает миллисекунды,
     * а APB1 = 72 МГц (типично для STM32F103).
     *
     * Пусть:
     *   - предделитель = 7200-1 → получаем 10 кГц (0.1 мс)
     *   - авто-перезагрузка = period_ms * 10 - 1
     */

    timer_set_prescaler(TIM4, 7200 - 1);
    timer_set_period(TIM4, period_ms * 10 - 1);

    timer_update_on_overflow(TIM4);
    timer_enable_irq(TIM4, TIM_DIER_UIE);

    /* Настраиваем NVIC */
    nvic_set_priority(NVIC_TIM4_IRQ, 9);
    nvic_enable_irq(NVIC_TIM4_IRQ);
}

void service_timer_enable(void) { timer_enable_counter(TIM4); }
void service_timer_disable(void) { timer_disable_counter(TIM4); }

void TIM4_Handler(void)
{
    timer_clear_flag(TIM4, TIM_SR_UIF);
    //debug_serial_printf("[%u] TIM\n", get_current_time_ms());

    event_bus_t *bus = event_dispatcher_get_bus();
    bool keep_unabled = false;

    keep_unabled = keep_unabled || generate_LED_update_event(bus);
    keep_unabled = keep_unabled || generate_USART_update_event(bus);

    if (!keep_unabled) { service_timer_disable(); }
}
