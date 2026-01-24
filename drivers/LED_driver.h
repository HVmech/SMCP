#pragma once

#ifndef SMCP_LED_DRIVER_H
#define SMCP_LED_DRIVER_H

#include <common/board.h>
#include <common/types.h>
#include <common/macro_debug.h>

#define LED_ON true
#define LED_OFF false

typedef struct { // Контрольная структура диода
    // s_MCU_pin?
    uint32_t port;
    uint16_t pin;
    bool inverted;
} s_LED_config;

bool LED_init(e_board_pin pin_code, bool inversion, s_LED_config* out_config); // Инициализация контрольной структуры
void LED_set(const s_LED_config* config, bool state); // Установка состояния ВКЛ / ВЫКЛ

inline void LED_on(const s_LED_config* config) {
    DEBUG_STATIC_CHECK_FALSE(config);
    LED_set(config, LED_ON);
}

inline void LED_off(const s_LED_config* config) {
    DEBUG_STATIC_CHECK_FALSE(config);
    LED_set(config, LED_OFF);
}

#endif // SMCP_LED_DRIVER_H
