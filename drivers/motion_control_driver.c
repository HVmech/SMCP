#include <drivers/motion_control_driver.h>

#include <common/debug_assert.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

static MCU_pin_t dir_pin_info;
static MCU_pin_t ena_pin_info;

bool motion_control_init(board_pin_e dir_pin, board_pin_e ena_pin) {
    DEBUG_ASSERT(dir_pin < PCNT && ena_pin < PCNT);

    // Определение пинов МК
    dir_pin_info = get_MCU_pin(dir_pin);
    ena_pin_info = get_MCU_pin(ena_pin);
    DEBUG_ASSERT(dir_pin_info.port && dir_pin_info.pin);
    DEBUG_ASSERT(ena_pin_info.port && ena_pin_info.pin);

    switch (ena_pin_info.port) { // Включение тактирования соответствующего порта для пина ena
        case GPIOA: { rcc_periph_clock_enable(RCC_GPIOA); break; }
        case GPIOB: { rcc_periph_clock_enable(RCC_GPIOB); break; }
        case GPIOC: { rcc_periph_clock_enable(RCC_GPIOC); break; }
        default: { return false; }
    }
    gpio_set_mode(ena_pin_info.port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, ena_pin_info.pin);

    switch (dir_pin_info.port) { // Включение тактирования соответствующего порта для пина dir
        case GPIOA: { rcc_periph_clock_enable(RCC_GPIOA); break; }
        case GPIOB: { rcc_periph_clock_enable(RCC_GPIOB); break; }
        case GPIOC: { rcc_periph_clock_enable(RCC_GPIOC); break; }
        default: { return false; }
    }
    gpio_set_mode(dir_pin_info.port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, dir_pin_info.pin);

    return true;
}

void set_motion_control_enable(bool state) {
    if (state) {
        gpio_clear(ena_pin_info.port, ena_pin_info.pin);
    } else {
        gpio_set(ena_pin_info.port, ena_pin_info.pin);
    }
}

void set_motion_control_direction(bool direction) {
    if (direction) {
        gpio_clear(dir_pin_info.port, dir_pin_info.pin);
    } else {
        gpio_set(dir_pin_info.port, dir_pin_info.pin);
    }
}