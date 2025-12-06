// services/led_service.c
#include <services/led_service.h>
#include <common/board.h>
#include <drivers/smcp_gpio.h>
#include <drivers/smcp_gp_timer.h>

static const smcp_gpio_t led = {LED_PORT, LED_PIN};
static const smcp_gp_timer_t led_timer = {LED_TIMER, LED_TIMER_IRQ};

typedef enum {
    LED_OFF,
    LED_ON,
    LED_BLINK
} led_state_t;

static led_state_t status = LED_ON;

// Callback для таймера
static void led_timer_callback(void) {
    smcp_gpio_toggle((smcp_gpio_t*)&led);
}

inline static void led_stop_blink(void) {
    smcp_gp_timer_stop(&led_timer);
}

void led_init(void) {
    smcp_gpio_init((smcp_gpio_t*)&led, SMCP_GPIO_MODE_OUTPUT_PP, SMCP_GPIO_2MHz);
    smcp_gpio_toggle((smcp_gpio_t*)&led);
}

void led_set(bool state) {
    if (status == LED_BLINK) { led_stop_blink(); }
    smcp_gpio_write((smcp_gpio_t*)&led, state);
    if (state) { status = LED_ON; } else { status = LED_OFF; }
}

void led_toggle(void) {
    if (status == LED_BLINK) { led_stop_blink(); }
    smcp_gpio_toggle((smcp_gpio_t*)&led);
    if (status == LED_ON) { status = LED_OFF; } else { status = LED_ON; }
}

void led_blink(uint32_t freq_hz) {
    if (status == LED_BLINK) { led_stop_blink(); }
    smcp_gp_timer_set_callback(&led_timer, led_timer_callback);
    smcp_gp_timer_init(&led_timer, freq_hz);
    smcp_gp_timer_start(&led_timer);
    status = LED_BLINK;
}

