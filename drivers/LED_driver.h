#pragma once

#ifndef SMCP_LED_DRIVER_H
#define SMCP_LED_DRIVER_H

#include <common/board.h>
#include <common/types.h>
#include <common/debug_assert.h>

typedef struct { // Контрольная структура диода
    // s_MCU_pin?
    uint32_t port;
    uint16_t pin;
    bool inverted;
} LED_t;

// Базовые функции
bool LED_init(board_pin_e pin_code, bool inversion, LED_t* out_config); // Инициализация контрольной структуры
void LED_set(const LED_t* config, bool state); // Установка состояния ВКЛ / ВЫКЛ

#endif // SMCP_LED_DRIVER_H
