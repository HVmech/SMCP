#pragma once
#ifndef SMCP_MATRIX_KEYBOARD_DRIVER_H
#define SMCP_MATRIX_KEYBOARD_DRIVER_H

#include <common/board.h>
#include <common/types.h>

#define KEYBOARD_ROWS 5
#define KEYBOARD_COLUMNS 4

#define SCAN_PERIOD_MS         1
#define DEBOUNCE_TICKS         7
#define REPEAT_DELAY_MS        500
#define REPEAT_PERIOD_MS       100
#define SCAN_IDLE_TIMEOUT_MS   30
#define KEYBOARD_TIMER_PRIORITY 4
#define KEYBOARD_TRIGGER_PRIORITY (KEYBOARD_TIMER_PRIORITY + 1)
#define RC_STABILITY_DELAY_TICK 200
#define GPIO_STABILITY_DELAY_MS 20

#define NORMAL_PRESS_DURATION_MS 500
#define LONG_PRESS_DURATION_MS 1000

typedef struct {
    board_pin_e column_pins[KEYBOARD_COLUMNS];
    board_pin_e row_pins[KEYBOARD_ROWS];
} matrix_keyboard_config_t;

typedef enum {
    KEY_F1    = 0,
    KEY_F2    = 1,
    KEY_SIGN  = 2,
    KEY_DOT   = 3,
    KEY_1     = 4,
    KEY_2     = 5,
    KEY_3     = 6,
    KEY_UP    = 7,
    KEY_4     = 8,
    KEY_5     = 9,
    KEY_6     = 10,
    KEY_DOWN  = 11,
    KEY_7     = 12,
    KEY_8     = 13,
    KEY_9     = 14,
    KEY_ESC   = 15,
    KEY_LEFT  = 16,
    KEY_0     = 17,
    KEY_RIGHT = 18,
    KEY_ENT   = 19,
    KEY_CNT
} key_t;

extern const uint8_t key_to_digit[KEY_CNT];

void matrix_keyboard_init(const matrix_keyboard_config_t *config);
uint8_t matrix_keyboard_key_to_digit(key_t key);

#endif // SMCP_MATRIX_KEYBOARD_DRIVER_H