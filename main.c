#include <drivers/smcp_gp_timer.h>
#include <services/led_service.h>
#include <common/board.h>

// Таймер для LED
static const smcp_gp_timer_t led_timer = {LED_TIMER, LED_TIMER_IRQ};

// Обработчик — вызывается из startup.s
void TIM2_Handler(void) {
    smcp_gp_timer_clear_irq(&led_timer);
    led_toggle();
}

int main(void) {
    led_init();
    smcp_gp_timer_init(&led_timer, 2); // 2 Гц → мигание 1 Гц
    smcp_gp_timer_start(&led_timer);

    while (1) { __asm("nop"); }
}