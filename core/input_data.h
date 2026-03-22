#pragma once
#ifndef SMCP_INPUT_DATA_H
#define SMCP_INPUT_DATA_H

#include <services/motion_controller_service.h>

#define INPUT_INTEGER_DIGITS 3
#define INPUT_FRACTIONAL_DIGITS (ANGLE_PRECISION - 1)
#define INPUT_DIGITS_COUNT (INPUT_INTEGER_DIGITS + INPUT_FRACTIONAL_DIGITS)

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
    bool mode;
    int32_t value;
    volatile bool error; // Флаг ошибки
    input_data_t data;
} input_context_t;

#endif // SMCP_INPUT_DATA_H