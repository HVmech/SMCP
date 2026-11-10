#pragma once
#ifndef SMCP_INPUT_DATA_H
#define SMCP_INPUT_DATA_H

#include <services/motion_controller_service.h>

#define INPUT_INTEGER_DIGITS 3
#define INPUT_FRACTIONAL_DIGITS (ANGLE_PRECISION - 1)
#define INPUT_DIGITS_COUNT (INPUT_INTEGER_DIGITS + INPUT_FRACTIONAL_DIGITS)

typedef enum {
    INPUT_CURRENT_ANGLE = 0,
    INPUT_RELATIVE_ANGLE,
    INPUT_CORRECTION_ANGLE
} input_mode_e;

typedef struct {
    // Целая часть
    uint8_t integer_digits[INPUT_INTEGER_DIGITS];
    uint8_t integer_count;
    
    // Дробная часть
    uint8_t fractional_digits[INPUT_FRACTIONAL_DIGITS];
    uint8_t fractional_count;
    
    uint8_t cursor_position;
    bool input_part; // Какая часть вводится (0 - целая, 1 - дробная)
    
    bool sign; // Знак (1 - минус, 0 - плюс)
} input_data_t;

typedef struct {
    input_mode_e mode;
    bool converted_to_steps;
    int32_t angle_value;
    input_data_t data;
    volatile bool error; // Флаг ошибки
} input_context_t;

#endif // SMCP_INPUT_DATA_H