#include <drivers/LED_driver.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

bool LED_init(board_pin_e pin_code, bool inversion, LED_t* out_config) {
    DEBUG_ASSERT(out_config);
    MCU_pin_t pin_info = get_MCU_pin(pin_code); // Определение пина МК
    DEBUG_ASSERT(pin_info.port && pin_info.pin);
    // Заполняем выходную структуру
    out_config->port = pin_info.port;
    out_config->pin = pin_info.pin;
    out_config->inverted = inversion;

    switch (pin_info.port) { // Включение тактирования соответствующего порта
        case GPIOA: { rcc_periph_clock_enable(RCC_GPIOA); break; }
        case GPIOB: { rcc_periph_clock_enable(RCC_GPIOB); break; }
        case GPIOC: { rcc_periph_clock_enable(RCC_GPIOC); break; }
        default: { return false; }
    }
    gpio_set_mode(pin_info.port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, pin_info.pin);
    return true;
}

void LED_set(const LED_t* config, bool state) {
    DEBUG_ASSERT(config);
    // state   : 1, 1, 0, 0
    // inverted: 0, 1, 0, 1
    // result  : 1, 0, 0, 1
    if (state ^ config->inverted) {
        gpio_set(config->port, config->pin);
    } else {
        gpio_clear(config->port, config->pin);
    }
}