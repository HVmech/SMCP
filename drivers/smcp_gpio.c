#include <drivers/smcp_gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

static uint8_t get_lib_speed(smcp_gpio_speed_t speed) {
    switch(speed) {
        case SMCP_GPIO_10MHz: {
            return GPIO_MODE_OUTPUT_10_MHZ;
        }
        case SMCP_GPIO_50MHz: {
            return GPIO_MODE_OUTPUT_50_MHZ;
        }
        default: {
            return GPIO_MODE_OUTPUT_2_MHZ;
        }
    }
}

static uint32_t port_to_rcc(uint8_t port) {
    switch (port) {
        case SMCP_GPIO_PORT_A: return RCC_GPIOA;
        case SMCP_GPIO_PORT_B: return RCC_GPIOB;
        case SMCP_GPIO_PORT_C: return RCC_GPIOC;
        default: return 0;
    }
}

static uint32_t port_to_base(uint8_t port) {
    switch (port) {
        case SMCP_GPIO_PORT_A: return GPIOA;
        case SMCP_GPIO_PORT_B: return GPIOB;
        case SMCP_GPIO_PORT_C: return GPIOC;
        default: return 0;
    }
}

void smcp_gpio_init(smcp_gpio_t *pin, smcp_gpio_mode_t mode, smcp_gpio_speed_t speed) {
    uint32_t rcc = port_to_rcc(pin->port);
    uint32_t base = port_to_base(pin->port);
    if (!rcc || !base) return;

    rcc_periph_clock_enable(rcc);

    uint8_t speed_lib, cnf;

    switch (mode) {
        case SMCP_GPIO_MODE_INPUT_FLOAT:
            speed_lib = GPIO_MODE_INPUT;
            cnf = GPIO_CNF_INPUT_FLOAT;
            break;
        case SMCP_GPIO_MODE_INPUT_PULLUP:
            speed_lib = GPIO_MODE_INPUT;
            cnf = GPIO_CNF_INPUT_PULL_UPDOWN;
            gpio_set(base, (1U << pin->pin));
            break;
        case SMCP_GPIO_MODE_INPUT_PULLDOWN:
            speed_lib = GPIO_MODE_INPUT;
            cnf = GPIO_CNF_INPUT_PULL_UPDOWN;
            gpio_clear(base, (1U << pin->pin));
            break;
        case SMCP_GPIO_MODE_INPUT_ANALOG:
            speed_lib = GPIO_MODE_INPUT;
            cnf = GPIO_CNF_INPUT_ANALOG;
            break;
        case SMCP_GPIO_MODE_OUTPUT_PP:
            speed_lib = get_lib_speed(speed);
            cnf = GPIO_CNF_OUTPUT_PUSHPULL;
            break;
        case SMCP_GPIO_MODE_OUTPUT_OD:
            speed_lib = get_lib_speed(speed);
            cnf = GPIO_CNF_OUTPUT_OPENDRAIN;
            break;
        default:
            return;
    }

    gpio_set_mode(base, speed_lib, cnf, (1U << pin->pin));
}

void smcp_gpio_write(const smcp_gpio_t *pin, bool value) {
    uint32_t base = port_to_base(pin->port);
    if (!base) return;
    if (value) gpio_set(base, (1U << pin->pin));
    else       gpio_clear(base, (1U << pin->pin));
}

bool smcp_gpio_read(const smcp_gpio_t *pin) {
    uint32_t base = port_to_base(pin->port);
    if (!base) return false;
    return gpio_get(base, (1U << pin->pin)) != 0;
}

void smcp_gpio_toggle(const smcp_gpio_t *pin) {
    uint32_t base = port_to_base(pin->port);
    if (!base) return;
    gpio_toggle(base, (1U << pin->pin));
}