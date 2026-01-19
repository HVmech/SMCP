#pragma once

#ifndef SMCP_LED_DRIVER_H
#define SMCP_LED_DRIVER_H

#include <common/types.h>

typedef uint8_t e_board_pin;

#define LED_ON true
#define LED_OFF false

typedef struct { // Контрольная структура диода
    uint32_t port;
    uint16_t pin;
    bool inverted;
} s_LED_config;

bool LED_init(e_board_pin pin_code, bool inversion, s_LED_config* out_config); // Инициализация контрольной структуры
void LED_set(const s_LED_config* config, bool state); // Установка состояния ВКЛ / ВЫКЛ
void LED_on(const s_LED_config* config); // ВКЛ
void LED_off(const s_LED_config* config); // ВЫКЛ

#endif // SMCP_LED_DRIVER_H
