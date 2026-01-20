#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <drivers/LED_driver.h>

bool LED_init(e_board_pin pin_code, bool inversion, s_LED_config* out_config) {
#ifdef DEBUG
    if (!out_config) { return false; } // Если передан пустой указатель
#endif

    s_MCU_pin pin_info = get_MCU_pin(pin_code); // Определение пина МК
#ifdef DEBUG
    if (pin_info.port == 0 || pin_info.pin == 0) { return false; } // Проверка корректности пина
#endif

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

void LED_set(const s_LED_config* config, bool state) {
    // state   : 1, 1, 0, 0
    // inverted: 0, 1, 0, 1
    // result  : 1, 0, 0, 1
    if (state ^ config->inverted) {
        gpio_set(config->port, config->pin);
    } else {
        gpio_clear(config->port, config->pin);
    }
}

inline void LED_on(const s_LED_config* config) {
    LED_set(config, LED_ON);
}

inline void LED_off(const s_LED_config* config) {
    LED_set(config, LED_OFF);
}