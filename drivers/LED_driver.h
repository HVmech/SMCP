#pragma once

#ifndef SMCP_LED_DRIVER_H
#define SMCP_LED_DRIVER_H

#include <common/board.h>
#include <common/types.h>
#include <common/debug_assert.h>

// Макросы для обозначения состояний диода
#define LED_ON true
#define LED_OFF false

typedef struct { // Контрольная структура диода
    // s_MCU_pin?
    uint32_t port;
    uint16_t pin;
    bool inverted;
} s_LED_config;

// Базовые функции
bool LED_init(e_board_pin pin_code, bool inversion, s_LED_config* out_config); // Инициализация контрольной структуры
void LED_set(const s_LED_config* config, bool state); // Установка состояния ВКЛ / ВЫКЛ

#endif // SMCP_LED_DRIVER_H
