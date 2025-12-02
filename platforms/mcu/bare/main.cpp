
#include "../../../components/drivers/gpio_driver.h"
#include "../../../components/drivers/timer_driver.h"
#include "../../../components/services/led_service.h"

void delay(t_INT32 ticks) {
    for(t_INT32 i = 0; i < ticks; i++) {
      asm("nop");
    }
}

int main(void)
{
  g_BUILT_IN_LED.init();

  while(1)
  {
    g_BUILT_IN_LED.set(n_LED_SERVICE::LED_ON);
    delay(100000000);
    g_BUILT_IN_LED.set(n_LED_SERVICE::LED_OFF);
    delay(100000000);
    g_BUILT_IN_LED.set(n_LED_SERVICE::LED_BLINK);
    g_BUILT_IN_LED.set_blink_period_ms(200);
    delay(100000000);
    g_BUILT_IN_LED.set_blink_period_ms(500);
    delay(100000000);
    g_BUILT_IN_LED.set_blink_period_ms(1000);
    delay(100000000);
  }
}

extern "C" {
  void TIM1_UP_Handler(void) __attribute__((used));

  void TIM1_UP_Handler(void)
  {
    n_TIM1::interrupt_handler();
  }
}