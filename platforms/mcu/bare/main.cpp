#include "../../../components/services/led_service.h"

void delay(volatile t_INT32 ticks) {
    for(t_INT32 i = 0; i < ticks; i++) {
        asm volatile("nop");
    }
}

int main(void) {
    g_BUILT_IN_LED.init();
    
    // 1. Отладочное мигание (3 коротких вспышки)
    for (int i = 0; i < 3; i++) {
        g_BUILT_IN_LED.set(n_LED_SERVICE::LED_ON);
        delay(100000);
        g_BUILT_IN_LED.set(n_LED_SERVICE::LED_OFF);
        delay(100000);
    }

    // 2. Статичные состояния (проверка GPIO)
    g_BUILT_IN_LED.set(n_LED_SERVICE::LED_ON);
    delay(1000000);
    
    g_BUILT_IN_LED.set(n_LED_SERVICE::LED_OFF);
    delay(500000);
    g_BUILT_IN_LED.set(n_LED_SERVICE::LED_ON);
    // 3. Мигание через TIM1
    g_BUILT_IN_LED.set_blink_period_ms(200);
    g_BUILT_IN_LED.set(n_LED_SERVICE::LED_BLINK);

    // 4. Бесконечный цикл
    while(1) {
        delay(20000000);
        g_BUILT_IN_LED.set(n_LED_SERVICE::LED_OFF);
        delay(20000000);
        g_BUILT_IN_LED.set(n_LED_SERVICE::LED_ON);
        delay(20000000);
        g_BUILT_IN_LED.set(n_LED_SERVICE::LED_BLINK);
    }
}

extern "C" {
  void TIM1_UP_Handler(void) __attribute__((used));

  void TIM1_UP_Handler(void)
  {
    n_TIM1::interrupt_handler();
  }
}