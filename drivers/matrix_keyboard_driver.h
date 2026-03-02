#pragma once
#ifndef SMCP_MATRIX_KEYBOARD_DRIVER_H
#define SMCP_MATRIX_KEYBOARD_DRIVER_H

#include <common/board.h>
#include <common/types.h>

#define KEYBOARD_ROWS 5
#define KEYBOARD_COLUMNS 4
//#define KEYBOARD_KEYS (KEYBOARD_ROWS * KEYBOARD_COLS)

#define SCAN_PERIOD_MS         1
#define DEBOUNCE_TICKS         7
#define REPEAT_DELAY_MS        500
#define REPEAT_PERIOD_MS       100
#define SCAN_IDLE_TIMEOUT_MS   30
#define KEYBOARD_TIMER_PRIORITY 9
#define KEYBOARD_TRIGGER_PRIORITY (KEYBOARD_TIMER_PRIORITY + 1)
#define RC_STABILITY_DELAY_TICK 200
#define GPIO_STABILITY_DELAY_MS 20

typedef struct {
    board_pin_e column_pins[KEYBOARD_COLUMNS];
    board_pin_e row_pins[KEYBOARD_ROWS];
} matrix_keyboard_config_t;

void matrix_keyboard_init(const matrix_keyboard_config_t *config);

#endif // SMCP_MATRIX_KEYBOARD_DRIVER_H