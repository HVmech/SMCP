#include <services/led_service.h>
#include <common/board.h>
#include <drivers/smcp_gpio.h>

static const smcp_gpio_t led = {LED_PORT, LED_PIN};

void led_init(void) {
    smcp_gpio_init((smcp_gpio_t*)&led, SMCP_GPIO_MODE_OUTPUT_PP, SMCP_GPIO_2MHz);
    led_off(); // active-low
}

void led_on(void)  { smcp_gpio_write(&led, false); }
void led_off(void) { smcp_gpio_write(&led, true); }
void led_toggle(void) { smcp_gpio_toggle(&led); }