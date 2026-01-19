#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <drivers/LED_driver.h>

bool LED_init(e_board_pin pin_code, bool inversion, s_LED_config* out_config) {
#ifdef DEBUG
    if (!out_config) { return false; } // Если передан пустой указатель
#endif

    uint32_t port = pin_code >> 4;
    uint16_t pin = 1 << (pin_code & 0x0F);

    switch (port) { // Доступные порты: A, B, C
        case 0: { port = GPIOA; break; }
        case 1: { port = GPIOB; break; }
        case 2: { port = GPIOC; break; }
        default: { return false; }
    }

    // DEBUG
    port = GPIOC;
    pin = GPIO13;

    // Заполняем выходную структуру
    out_config->port = port;
    out_config->pin = pin;
    out_config->inverted = inversion;

    // Включаем тактирование порта GPIOC
    rcc_periph_clock_enable(RCC_GPIOC);

    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
                 GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

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