#pragma once
#include <common/types.h>

typedef enum {
    SMCP_GPIO_PORT_A = 0,
    SMCP_GPIO_PORT_B = 1,
    SMCP_GPIO_PORT_C = 2,
} smcp_gpio_port_t;

typedef enum {
    SMCP_GPIO_MODE_INPUT_FLOAT = 0,
    SMCP_GPIO_MODE_INPUT_PULLUP = 1,
    SMCP_GPIO_MODE_INPUT_PULLDOWN = 2,
    SMCP_GPIO_MODE_INPUT_ANALOG = 3,
    SMCP_GPIO_MODE_OUTPUT_PP = 4,
    SMCP_GPIO_MODE_OUTPUT_OD = 5
} smcp_gpio_mode_t;

typedef enum {
    SMCP_GPIO_10MHz = 0,
    SMCP_GPIO_2MHz = 1,
    SMCP_GPIO_50MHz = 2
} smcp_gpio_speed_t;

typedef struct {
    uint8_t port;
    uint8_t pin;
} smcp_gpio_t;

void smcp_gpio_init(smcp_gpio_t *pin, smcp_gpio_mode_t mode, smcp_gpio_speed_t speed);
void smcp_gpio_write(const smcp_gpio_t *pin, bool value);
bool smcp_gpio_read(const smcp_gpio_t *pin);
void smcp_gpio_toggle(const smcp_gpio_t *pin);