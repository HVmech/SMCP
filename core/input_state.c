#include "core/input_data.h"
#include <core/input_state.h>

#include <core/app_context.h>
#include <core/event_dispatcher.h>
#include <core/event.h>

#include <services/debug_serial_service.h>
#include <services/LCD_service.h>
#include <services/motion_controller_service.h>

#include <drivers/matrix_keyboard_driver.h>
#include <drivers/time_driver.h>

#include <common/debug_assert.h>
#include <common/utils.h>
#include <stdint.h>
/*
//#define INPUT_PRESICION ((ANGLE_PRECISION - 1) >= 0 ? (ANGLE_PRECISION - 1) : 1)
#define INPUT_OFFSET_DIGITS 0
#define INPUT_SIGN_DIGITS 1
#define INPUT_SEPARATOR_DIGITS 1
#define INPUT_INTEGER_DIGITS 3
#define INPUT_FRACTIONAL_DIGITS ANGLE_PRECISION
#define INPUT_DIGITS_COUNT (INPUT_INTEGER_DIGITS + INPUT_FRACTIONAL_DIGITS)

static uint8_t input_digits[INPUT_DIGITS_COUNT];
static uint8_t saved_integer_digits;
static bool sign;

void input_state_enter(void);
void input_state_exit(void);
void input_state_event_handler(const event_t *evt);

static inline void input_state_display(bool first_time) {
    if (first_time) {
        if (app_context.input_data.mode) {
            LCD_set_string(0, 0, "RELATIVE ANGLE:", false);
        }
        else {
            LCD_set_string(0, 0, "NEW ANGLE:", false);
        }
    }
    LCD_display_angle(1, INPUT_OFFSET_DIGITS, app_context.input_data.value, true, app_context.input_data.mode);
    LCD_update_request();
}

static inline void input_state_clear_all_digits(void) {
    app_context.input_data.current_digit = 0;
    app_context.input_data.value = 0;
    memset(&input_digits, 0, INPUT_DIGITS_COUNT);
    saved_integer_digits = 0;
    sign = false;
}

static inline bool input_state_clear_digit(void) {
    const bool integer_digits_input_in_process = app_context.input_data.current_digit < INPUT_INTEGER_DIGITS;
    if (integer_digits_input_in_process) {
        const bool higher_digit_is_set = app_context.input_data.current_digit > INPUT_INTEGER_DIGITS - saved_integer_digits;
        if (higher_digit_is_set) {
            input_digits[app_context.input_data.current_digit] = 0;
            --app_context.input_data.current_digit;
            --saved_integer_digits;
        }
        else {
            if (input_digits[app_context.input_data.current_digit] == 0) {
                bool any_digit_is_set = false;
                for (uint8_t i = 0; i < INPUT_DIGITS_COUNT; ++i) {
                    if (input_digits[app_context.input_data.current_digit] != 0) { any_digit_is_set = true; break; }
                }

                if (any_digit_is_set) {
                    input_state_clear_all_digits();
                }
                else {
                    return true;
                }
            }
            else {
                input_digits[app_context.input_data.current_digit] = 0;
                app_context.input_data.current_digit = 0;
                saved_integer_digits = 0;
            }
            
        }
    }
    else {
        input_digits[app_context.input_data.current_digit] = 0;
        --app_context.input_data.current_digit;
    }
    return false;
}

static inline void input_state_update_value(void) {
    app_context.input_data.value = 0;
    for (uint8_t i = 0; i < INPUT_DIGITS_COUNT; ++i) {
        app_context.input_data.value *= 10;
        app_context.input_data.value += input_digits[i];
    }
    app_context.input_data.value *= sign ? -1 : 1;
}

static inline void input_state_set_current_digit(uint8_t digit) {
    DEBUG_ASSERT(digit < 10);

    do {
        // Если заполнены не все разряды
        const bool all_digits_filled = app_context.input_data.current_digit < INPUT_DIGITS_COUNT;
        
        if (!all_digits_filled) {
            // Если введены не все разряды целой части
            const bool integer_digits_input_in_process = app_context.input_data.current_digit < INPUT_INTEGER_DIGITS;
            
            if (integer_digits_input_in_process) {
                // Если число сохраненных разрядов целой части меньше числа целых разрядов и происходит ввод нового разряда
                const bool some_integer_digits_are_empty = saved_integer_digits < INPUT_INTEGER_DIGITS;
                const bool current_integer_digit_is_empty = some_integer_digits_are_empty && app_context.input_data.current_digit == saved_integer_digits;
                
                if (current_integer_digit_is_empty) {
                    // Проверка возможности ввода данного значения в текущий разряд
                    const bool first_digit_invalid = input_digits[1] > 3;
                    const bool second_digit_invalid = input_digits[1] == 3 && input_digits[2] > 5;
                    if (first_digit_invalid || second_digit_invalid) {
                        ++app_context.input_data.current_digit;
                        continue; // Переход к следующему разряду
                    }

                    const bool all_digits_are_zeros = input_digits[0] == 0 && input_digits[1] == 0;
                    
                    if (!all_digits_are_zeros) {
                        // Сдвиг введенных ранее разрядов
                        for (uint8_t i = 0; i < INPUT_INTEGER_DIGITS - 1; ++i) {
                            input_digits[i] = input_digits[i + 1];
                        }

                        // Запись данного значения в текущий разряд
                        input_digits[INPUT_INTEGER_DIGITS - 1] = digit;

                        ++app_context.input_data.current_digit;
                        ++saved_integer_digits;
                    }
                }
                else { // Обновление старого разряда
                    // Проверка возможности ввода данного значения в текущий разряд
                    const bool first_digit_invalid = app_context.input_data.current_digit == 0 && (digit == 0 || digit > 3);
                    const bool second_digit_invalid = app_context.input_data.current_digit == 1 && ((input_digits[0] == 3 && digit > 5) || (input_digits[0] == 0 && digit == 0));
                    
                    if (!(first_digit_invalid || second_digit_invalid)) {
                        input_digits[app_context.input_data.current_digit] = digit;
                        ++app_context.input_data.current_digit;
                    }
                }
            }
            else { // Ввод разрядов дробной части
                input_digits[app_context.input_data.current_digit] = digit;
                ++app_context.input_data.current_digit;
            }
        }
    } while(0);
}

static inline void input_state_manage_subscriptions(bool state) {
    event_bus_t *bus = event_dispatcher_get_bus();

    if (state) {
        event_bus_subscribe(bus, EVENT_KEY_PRESS, input_state_event_handler);
        event_bus_subscribe(bus, EVENT_KEY_RELEASE, input_state_event_handler);
    }
    else {
        event_bus_unsubscribe(bus, EVENT_KEY_PRESS, input_state_event_handler);
        event_bus_unsubscribe(bus, EVENT_KEY_RELEASE, input_state_event_handler);
    }
}

void input_state_enter(void) {
    input_state_clear_all_digits();
    input_state_display(true);
    input_state_manage_subscriptions(true);
    debug_serial_printf("INPUT\n");
}

void input_state_exit(void) {
    input_state_manage_subscriptions(false);
    debug_serial_printf("STATE: INPUT --> ");
}

void input_state_event_handler(const event_t *evt) {
    DEBUG_ASSERT(evt);

    switch (evt->id) {
        case EVENT_KEY_PRESS: {
            const key_t key = evt->payload.data.unsigned_value;

            switch (key) {
                case KEY_0:
                case KEY_1:
                case KEY_2:
                case KEY_3:
                case KEY_4:
                case KEY_5:
                case KEY_6:
                case KEY_7:
                case KEY_8:
                case KEY_9: {
                    input_state_set_current_digit(matrix_keyboard_key_to_digit(key));
                    break;
                }
                case KEY_DOT: {
                    const bool integer_digits_input_in_process = app_context.input_data.current_digit < INPUT_INTEGER_DIGITS;
                    if (integer_digits_input_in_process) {
                        app_context.input_data.current_digit = INPUT_INTEGER_DIGITS;

                        const bool no_integer_digits_set = saved_integer_digits == 0;
                        if (no_integer_digits_set) { saved_integer_digits = 1; }
                    }
                    break;
                }
                case KEY_UP: {
                    const bool max_digit_num_reached = input_digits[app_context.input_data.current_digit] == 9;
                    if (max_digit_num_reached) {
                        input_digits[app_context.input_data.current_digit] = 0;
                    }
                    else {
                        ++input_digits[app_context.input_data.current_digit];
                    }
                    break;
                }
                case KEY_DOWN: {
                    const bool min_digit_num_reached = input_digits[app_context.input_data.current_digit] == 0;
                    if (min_digit_num_reached) {
                        input_digits[app_context.input_data.current_digit] = 9;
                    }
                    else {
                        --input_digits[app_context.input_data.current_digit];
                    }
                    break;
                }
                case KEY_RIGHT: {
                    input_state_set_current_digit(input_digits[app_context.input_data.current_digit]);
                    break;
                }
                case KEY_LEFT: {
                    const bool integer_digits_input_in_process = app_context.input_data.current_digit < INPUT_INTEGER_DIGITS;
                    if (integer_digits_input_in_process) {
                        const bool higher_digit_is_set = app_context.input_data.current_digit > INPUT_INTEGER_DIGITS - saved_integer_digits;
                        if (higher_digit_is_set) {
                            --app_context.input_data.current_digit;
                        }
                    }
                    else {
                        --app_context.input_data.current_digit;
                    }
                    break;
                }
                case KEY_SIGN: {
                    if (app_context.input_data.mode) {
                        sign = !sign;
                    }
                    break;
                }
                default: { return; }
            }
            break;
        }
        case EVENT_KEY_RELEASE: {
            const key_t key = evt->payload.data.unsigned_value & ((1 << 8) - 1);
            const uint32_t duration = evt->payload.data.unsigned_value >> 8;

            switch (key) {
                case KEY_ESC: {
                    if (duration >= LONG_PRESS_DURATION_MS) {
                        input_state_clear_all_digits();
                    }
                    else {
                        if (input_state_clear_digit()) {
                            app_state_transition_request(APP_STATE_INPUT);
                        }
                    }
                    break;
                }
                case KEY_ENT: {
                    if (duration <= NORMAL_PRESS_DURATION_MS) {
                        app_state_transition_request(APP_STATE_ACTIVE);
                    }
                    break;
                }
                default: { return; }
            }
            break;
        }
        default: { return; }
    }
    input_state_update_value();
    input_state_display(false);
}


typedef struct {
    // Целая часть
    uint8_t integer_digits[INPUT_INTEGER_DIGITS];
    uint8_t integer_count;
    
    // Дробная часть
    uint8_t fractional_digits[INPUT_FRACTIONAL_DIGITS];
    uint8_t fractional_count;
    
    uint8_t cursor_position;
    uint8_t input_part; // Какая часть вводится (0 - целая, 1 - дробная)
    
    uint8_t sign; // Знак (1 - минус, 0 - плюс)
    
    bool error; // Флаг ошибки
} input_data_t;

*/

// Функции отображения
static inline void input_state_update_display(bool first_time) {
    if (first_time) {
        if (app_context.input_context.mode) {
            LCD_set_string(0, 0, "RELATIVE ANGLE:", false, false);
        }
        else {
            LCD_set_string(0, 0, "NEW ANGLE:", false, false);
        }
    }
    LCD_display_angle(1, 15, app_context.input_context.value, app_context.input_context.mode, false);
    // TODO: Отобразить курсор
    LCD_update_request();
}

static inline bool input_state_check_any_digit_set(void) {
    return (app_context.input_context.data.integer_count || app_context.input_context.data.fractional_count);
}

// Функции обновления параметров ввода
static inline void input_state_reset_all_digits(void) {
    memset(&app_context.input_context.data, 0, sizeof(app_context.input_context.data));
}

// Сброс значения
static inline void input_state_reset_value(void) {
    app_context.input_context.value = 0;
}

// Проверка ввода
static inline bool input_state_validate_input(void) {
    return app_context.input_context.value > -3600000 && app_context.input_context.value < 3600000;
}

// Обработка точки
static inline void input_state_process_dot() {
    if (app_context.input_context.data.input_part == 0 && app_context.input_context.data.fractional_count == 0) {
        app_context.input_context.data.input_part = 1;
        app_context.input_context.data.cursor_position = 0;
    }
}

// Сдвиг курсора вперед/назад
static inline void input_state_process_cursor_shift(bool forward) {
    if (app_context.input_context.data.input_part) {
        if (forward) {
            if (app_context.input_context.data.cursor_position < INPUT_FRACTIONAL_DIGITS) {
                ++app_context.input_context.data.cursor_position;
            }
        }
        else {
            if (app_context.input_context.data.cursor_position > 0) {
                --app_context.input_context.data.cursor_position;
            }
            else {
                app_context.input_context.data.input_part = false;

                if (app_context.input_context.data.fractional_count || app_context.input_context.data.integer_count == INPUT_INTEGER_DIGITS) {
                    app_context.input_context.data.cursor_position = INPUT_INTEGER_DIGITS - 1;
                }
                else {
                    app_context.input_context.data.cursor_position = INPUT_INTEGER_DIGITS;
                }
            }
        }
    }
    else {
        if (forward) {
            if (app_context.input_context.data.cursor_position < INPUT_INTEGER_DIGITS) {
                ++app_context.input_context.data.cursor_position;

                if (app_context.input_context.data.cursor_position == INPUT_INTEGER_DIGITS && (app_context.input_context.data.fractional_count || app_context.input_context.data.integer_count == INPUT_INTEGER_DIGITS)) {
                    app_context.input_context.data.input_part = true;
                    app_context.input_context.data.cursor_position = 0;
                }
            }
            else {
                app_context.input_context.data.input_part = true;
                app_context.input_context.data.cursor_position = 1;
            }
        }
        else {
            if (app_context.input_context.data.cursor_position > 0) {
                --app_context.input_context.data.cursor_position;
            }
        }
    }
}

// Смена знака
static inline void input_state_process_sign_change(void) {
    if (app_context.input_context.mode) {
        if (input_state_check_any_digit_set()) {
            app_context.input_context.data.sign = !app_context.input_context.data.sign;
        }
    }
}

// Получение номера первого ненулевого разряда целой части
static inline uint8_t input_state_search_first_integer_digit(void) {
    for (uint8_t i = 0; i < INPUT_INTEGER_DIGITS; ++i) {
        if (app_context.input_context.data.integer_digits[i] != 0) {
            return i;
        }
    }
    return INPUT_INTEGER_DIGITS;
}

// Обновление числа введенных разрядов целой части
static inline void input_state_update_integer_digits_count(uint8_t first_digit) {
    app_context.input_context.data.integer_count = INPUT_INTEGER_DIGITS - first_digit;
}

// Сдвиг разрядов целой части влево/вправо от текущей позиции включительно
static inline void input_state_integer_digits_shift(uint8_t from_pos, bool shift_left) {
    DEBUG_ASSERT(INPUT_INTEGER_DIGITS);

    if (INPUT_INTEGER_DIGITS == 1) { return; }
    if (from_pos >= INPUT_INTEGER_DIGITS) { return; }

    if (shift_left) {
        for (uint8_t i = from_pos; i < INPUT_INTEGER_DIGITS - 1; ++i) {
            app_context.input_context.data.integer_digits[i] = app_context.input_context.data.integer_digits[i + 1];
        }
    }
    else {
        for (uint8_t i = from_pos; i > 0; --i) {
            app_context.input_context.data.integer_digits[i] = app_context.input_context.data.integer_digits[i - 1];
        }
    }
}

// Сдвиг разрядов дробной части влево/вправо от текущей позиции включительно
static inline void input_state_fractional_digits_shift(uint8_t from_pos, bool shift_left) {
    DEBUG_ASSERT(INPUT_FRACTIONAL_DIGITS);

    if (INPUT_FRACTIONAL_DIGITS == 1) { return; }
    if (from_pos >= INPUT_FRACTIONAL_DIGITS) { return; }

    if (shift_left) {
        for (uint8_t i = from_pos; i < INPUT_FRACTIONAL_DIGITS - 1; ++i) {
            app_context.input_context.data.fractional_digits[i] = app_context.input_context.data.fractional_digits[i + 1];
        }
    }
    else {
        for (uint8_t i = from_pos; i > 0; --i) {
            app_context.input_context.data.fractional_digits[i] = app_context.input_context.data.fractional_digits[i - 1];
        }
    }
}

// Получение номера последнего ненулевого разряда дробной части
static inline uint8_t input_state_search_last_fractional_digit(void) {
    DEBUG_ASSERT(INPUT_FRACTIONAL_DIGITS);

    for (uint8_t i = INPUT_FRACTIONAL_DIGITS; i > 0; --i) {
        if (app_context.input_context.data.fractional_digits[i - 1] != 0) {
            return i - 1;
        }
    }
    return 0;
}

// Обновление числа введенных разрядов целой части
static inline void input_state_update_fractional_digits_count(uint8_t last_digit) {
    app_context.input_context.data.fractional_count = last_digit == 0 && app_context.input_context.data.fractional_digits[0] == 0 ? 0 : last_digit + 1;
}

static inline void input_state_update_value(void) {
    input_state_reset_value();

    for (uint8_t i = 0; i < INPUT_INTEGER_DIGITS; ++i) {
        app_context.input_context.value *= 10;
        app_context.input_context.value += app_context.input_context.data.integer_digits[i];
    }

    for (uint8_t i = 0; i < INPUT_FRACTIONAL_DIGITS; ++i) {
        app_context.input_context.value *= 10;
        app_context.input_context.value += app_context.input_context.data.fractional_digits[i];
    }
}

// Обновление данных целой части
static inline void input_state_update_integer_data(void) {
    const uint8_t first_digit = input_state_search_first_integer_digit();
    input_state_update_integer_digits_count(first_digit);
}

// Обновление данных дробной части
static inline void input_state_update_fractional_data(void) {
    const uint8_t last_digit = input_state_search_last_fractional_digit();
    input_state_update_fractional_digits_count(last_digit);
}

// Обработка ввода цифрового разряда
static inline void input_state_process_digit_set(uint8_t digit) {
    if (app_context.input_context.data.input_part) {
        if (app_context.input_context.data.cursor_position < INPUT_FRACTIONAL_DIGITS) {
            app_context.input_context.data.fractional_digits[app_context.input_context.data.cursor_position] = digit;
            input_state_process_cursor_shift(true);
        }

        input_state_update_fractional_data();
    }
    else {
        uint8_t first_digit = INPUT_INTEGER_DIGITS - app_context.input_context.data.integer_count;

        if (app_context.input_context.data.cursor_position < first_digit) {
            app_context.input_context.data.integer_digits[app_context.input_context.data.cursor_position] = digit;
            input_state_process_cursor_shift(true);
        }
        else {
            if (app_context.input_context.data.cursor_position == INPUT_INTEGER_DIGITS) {
                if (app_context.input_context.data.integer_count < INPUT_INTEGER_DIGITS) {
                    input_state_integer_digits_shift(0, true);
                    app_context.input_context.data.integer_digits[INPUT_INTEGER_DIGITS - 1] = digit;

                    input_state_update_integer_data();

                    if (app_context.input_context.data.integer_count == INPUT_INTEGER_DIGITS) {
                        app_context.input_context.data.input_part = true;
                        app_context.input_context.data.cursor_position = 0;
                    }
                }
                else { // TODO: Удалить после тестирования
                    debug_serial_printf("ERROR!!!");
                    delay_ms(100000);
                }
            }
            else {
                app_context.input_context.data.integer_digits[app_context.input_context.data.cursor_position] = digit;
                input_state_process_cursor_shift(true);
            }
        }

        input_state_update_integer_data();
    }
}

// Циклическое изменение целого разряда
static inline void input_state_process_integer_digit_cyclic_change(bool increment) {
    const uint8_t active_digit = app_context.input_context.data.cursor_position;

    if (active_digit != INPUT_INTEGER_DIGITS) {
        if (increment) {
            if (app_context.input_context.data.integer_digits[active_digit] == 9) {
                app_context.input_context.data.integer_digits[active_digit] = 0;
            }
            else {
                ++app_context.input_context.data.integer_digits[active_digit];
            }
        }
        else {
            if (app_context.input_context.data.integer_digits[active_digit] == 0) {
                app_context.input_context.data.integer_digits[active_digit] = 9;
            }
            else {
                --app_context.input_context.data.integer_digits[active_digit];
            }
        }

        input_state_update_integer_data();
    }
}

// Циклическое изменение дробного разряда
static inline void input_state_process_fractional_digit_cyclic_change(bool increment) {
    const uint8_t active_digit = app_context.input_context.data.cursor_position;

    if (active_digit != INPUT_FRACTIONAL_DIGITS && active_digit < app_context.input_context.data.fractional_count) {
        if (increment) {
            if (app_context.input_context.data.fractional_digits[active_digit] == 9) {
                if (active_digit + 1 == app_context.input_context.data.fractional_count) {
                    app_context.input_context.data.fractional_digits[active_digit] = 1;
                }
                else {
                    app_context.input_context.data.fractional_digits[active_digit] = 0;
                }
            }
            else {
                ++app_context.input_context.data.fractional_digits[active_digit];
            }
        }
        else {
            if (app_context.input_context.data.fractional_digits[active_digit] == 0 || (active_digit + 1 == app_context.input_context.data.fractional_count && app_context.input_context.data.fractional_digits[active_digit] == 1)) {
                app_context.input_context.data.fractional_digits[active_digit] = 9;
            }
            else {
                --app_context.input_context.data.fractional_digits[active_digit];
            }
        }

        input_state_update_fractional_data();
    }
}

// Циклическое изменения активного разряда
static inline void input_state_process_digit_cyclic_change(bool increment) {
    if (app_context.input_context.data.input_part) {
        input_state_process_fractional_digit_cyclic_change(increment);
    }
    else {
        input_state_process_integer_digit_cyclic_change(increment);
    }
}

// Обработка сброса разряда
static inline void input_state_process_digit_reset(void) {
    if (app_context.input_context.data.input_part) {
        if (app_context.input_context.data.cursor_position) {
            input_state_fractional_digits_shift(app_context.input_context.data.cursor_position - 1, true);
            app_context.input_context.data.fractional_digits[INPUT_FRACTIONAL_DIGITS - 1] = 0;
            input_state_process_cursor_shift(false);
        }
        else {
            input_state_integer_digits_shift(INPUT_INTEGER_DIGITS - 1, false);
            app_context.input_context.data.integer_digits[0] = 0;
        }
    }
    else {
        if (app_context.input_context.data.integer_count == 0 && app_context.input_context.data.cursor_position == INPUT_INTEGER_DIGITS) {
            app_context.input_context.data.cursor_position = INPUT_INTEGER_DIGITS - 1;
        }
        else {
            input_state_integer_digits_shift(app_context.input_context.data.cursor_position - 1, false);
            app_context.input_context.data.integer_digits[0] = 0;
        }
    }

    input_state_update_integer_data();
    input_state_update_fractional_data();
}

static inline void input_state_process_apply() {
    const bool validation = input_state_validate_input();

    if (validation) {
        app_state_transition_request(APP_STATE_ACTIVE);
    }
    else {
        // TODO: Сформировать ошибку ввода
        //input_state_clear_all_digits();
    }
}

static inline void input_state_cancel_input(void) {
    app_state_transition_request(APP_STATE_IDLE);
}

// Обработчик событий ввода
void input_state_event_handler(const event_t *evt) {
    DEBUG_ASSERT(evt);

    switch (evt->id) {
        case EVENT_KEY_PRESS: {
            const key_t key = evt->payload.data.unsigned_value;

            switch (key) {
                case KEY_0:
                case KEY_1:
                case KEY_2:
                case KEY_3:
                case KEY_4:
                case KEY_5:
                case KEY_6:
                case KEY_7:
                case KEY_8:
                case KEY_9: {
                    const uint8_t digit = matrix_keyboard_key_to_digit(key);
                    input_state_process_digit_set(digit);
                    break;
                }
                case KEY_DOT: {
                    input_state_process_dot();
                    break;
                }
                case KEY_UP: {
                    input_state_process_digit_cyclic_change(true);
                    break;
                }
                case KEY_DOWN: {
                    input_state_process_digit_cyclic_change(false);
                    break;
                }
                case KEY_RIGHT: {
                    input_state_process_cursor_shift(true);
                    break;
                }
                case KEY_LEFT: {
                    input_state_process_cursor_shift(false);
                    break;
                }
                case KEY_SIGN: {
                    input_state_process_sign_change();
                    break;
                }
                case KEY_ESC: {
                    if (input_state_check_any_digit_set()) {
                        input_state_process_digit_reset();
                    }
                    else {
                        input_state_cancel_input();
                    }
                    break;
                }
                default: { return; }
            }
            break;
        }
        case EVENT_KEY_REPEAT: {
            const key_t key = evt->payload.data.unsigned_value;
            switch (key) {
                case KEY_0:
                case KEY_1:
                case KEY_2:
                case KEY_3:
                case KEY_4:
                case KEY_5:
                case KEY_6:
                case KEY_7:
                case KEY_8:
                case KEY_9: {
                    const uint8_t digit = matrix_keyboard_key_to_digit(key);
                    input_state_process_digit_set(digit);
                    break;
                }
                case KEY_UP: {
                    input_state_process_digit_cyclic_change(true);
                    break;
                }
                case KEY_DOWN: {
                    input_state_process_digit_cyclic_change(false);
                    break;
                }
                case KEY_RIGHT: {
                    input_state_process_cursor_shift(true);
                    break;
                }
                case KEY_LEFT: {
                    input_state_process_cursor_shift(false);
                    break;
                }
                case KEY_ESC: {
                    input_state_cancel_input();
                    break;
                }
                default: { return; }
            }
            break;
        }
        case EVENT_KEY_RELEASE: {
            const key_t key = evt->payload.data.unsigned_value & ((1 << 8) - 1);
            const uint32_t duration = evt->payload.data.unsigned_value >> 8;

            switch (key) {
                case KEY_ENT: {
                    if (duration <= LONG_PRESS_DURATION_MS) {
                        input_state_process_apply();
                    }
                    break;
                }
                default: { return; }
            }
            break;
        }
        default: { return; }
    }
    input_state_update_value();
    input_state_update_display(false);
}

// Функция обновления подписок
static inline void input_state_manage_subscriptions(bool state) {
    event_bus_t *bus = event_dispatcher_get_bus();

    if (state) {
        event_bus_subscribe(bus, EVENT_KEY_PRESS, input_state_event_handler);
        event_bus_subscribe(bus, EVENT_KEY_REPEAT, input_state_event_handler);
        event_bus_subscribe(bus, EVENT_KEY_RELEASE, input_state_event_handler);
    }
    else {
        event_bus_unsubscribe(bus, EVENT_KEY_PRESS, input_state_event_handler);
        event_bus_unsubscribe(bus, EVENT_KEY_REPEAT, input_state_event_handler);
        event_bus_unsubscribe(bus, EVENT_KEY_RELEASE, input_state_event_handler);
    }
}

void input_state_enter(void) {
    input_state_reset_all_digits();
    input_state_reset_value();
    input_state_update_display(true);
    input_state_manage_subscriptions(true);
    debug_serial_printf("INPUT\n");
}

void input_state_exit(void) {
    input_state_manage_subscriptions(false);
    debug_serial_printf("STATE: INPUT --> ");
}

const app_state_t input_state = {
    .enter = input_state_enter,
    .exit = input_state_exit,
    .event_handler = input_state_event_handler
};