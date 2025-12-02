
/*
#include "../../../components/drivers/gpio_driver.h"
#include "../../../components/drivers/timer_driver.h"
#include "../../../components/services/led_service.h"

void delay(volatile t_INT32 ticks) {
    for(t_INT32 i = 0; i < ticks; i++) {
      asm("nop");
    }
}

int main(void)
{
  g_BUILT_IN_LED.init();

  while(1) {
    g_BUILT_IN_LED.set(n_LED_SERVICE::LED_ON);
    delay(1000000);
    g_BUILT_IN_LED.set(n_LED_SERVICE::LED_OFF);
    delay(1000000);
    g_BUILT_IN_LED.set(n_LED_SERVICE::LED_BLINK);
    g_BUILT_IN_LED.set_blink_period_ms(200);
    delay(1000000);
    g_BUILT_IN_LED.set_blink_period_ms(500);
    delay(1000000);
    g_BUILT_IN_LED.set_blink_period_ms(1000);
    delay(1000000);
  }
}

extern "C" {
  void TIM1_UP_Handler(void) __attribute__((used));

  void TIM1_UP_Handler(void)
  {
    n_TIM1::interrupt_handler();
  }
}
*/

#include "registers.h"
#include "../../../components/services/led_service.h"

void delay(volatile t_INT32 ticks) {
    for(t_INT32 i = 0; i < ticks; i++) {
        asm("nop");
    }
}

int main(void) {
    g_BUILT_IN_LED.init(); // Теперь без TIM1 в init()
    
    // Отладочное мигание через сервис
    for (int i = 0; i < 3; i++) {
        g_BUILT_IN_LED.set(n_LED_SERVICE::LED_ON);  // ON = false (активный низкий)
        delay(100000);  // ~0.1 сек
        g_BUILT_IN_LED.set(n_LED_SERVICE::LED_OFF); // OFF = true
        delay(100000);
    }

    // Основной цикл: простое мигание через сервис
    while(1) {
        g_BUILT_IN_LED.set(n_LED_SERVICE::LED_ON);
        delay(500000);  // ~0.5 сек при 8 МГц
        
        g_BUILT_IN_LED.set(n_LED_SERVICE::LED_OFF);
        delay(500000);
    }
}