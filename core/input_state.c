#include <core/input_state.h>

#include <globals/keyboard_globals.h>

#include <core/app_state.h>
#include <core/app_context.h>
#include <core/input_data.h>
#include <core/service_timer.h>
#include <core/event_dispatcher.h>
#include <core/event.h>

#include <services/debug_serial_service.h>
#include <services/LCD_service.h>
#include <services/motion_controller_service.h>

#include <drivers/matrix_keyboard_driver.h>
#include <drivers/time_driver.h>

#include <common/debug_assert.h>
#include <common/utils.h>
#include <common/types.h>

#define CONST_INPUT_STATE_MODE_INFO_ROW 0
#define CONST_INPUT_STATE_ACTIVE_ROW 1

static inline void input_state_display_angle(uint8_t row, bool right_alignment) {
    DEBUG_ASSERT(row < LCD_HEIGHT);

    const uint8_t sign_fields = app_context.input_context.mode ? 1 : 0;

    const uint8_t integer_cursor = app_context.input_context.data.input_part ? 0 : (INPUT_INTEGER_DIGITS - app_context.input_context.data.cursor_position);
    const uint8_t integer_fields = MAX_VALUE((MAX_VALUE(app_context.input_context.data.integer_count, 1)), integer_cursor);

    const uint8_t fractional_cursor = app_context.input_context.data.input_part ? app_context.input_context.data.cursor_position : 0;
    const uint8_t fractional_pos = MAX_VALUE(app_context.input_context.data.fractional_count, fractional_cursor);
    const bool fractional_pos_source = fractional_cursor >= app_context.input_context.data.fractional_count;
    const uint8_t fractional_fields = app_context.input_context.data.input_part ? (fractional_pos_source ? MIN_VALUE(fractional_pos + 1, INPUT_FRACTIONAL_DIGITS) : fractional_pos) : fractional_pos;

    const uint8_t dot_fields = fractional_fields ? 1 : 0;

    const uint8_t measurement_fields = 1;

    const uint8_t all_angle_fields = sign_fields + integer_fields + dot_fields + fractional_fields + measurement_fields;

    const uint8_t start_pos = right_alignment ? LCD_LENGTH - all_angle_fields : 0;
    uint8_t pos = start_pos;

    if (sign_fields) {
        const unsigned char sign_char = app_context.input_context.data.sign ? LCD_CHAR_MINUS : LCD_CHAR_PLUS; 
        LCD_set_char(row, pos++, sign_char, false);
    }
    
    DEBUG_ASSERT(INPUT_INTEGER_DIGITS);
    DEBUG_ASSERT(integer_fields <= INPUT_INTEGER_DIGITS);
    for (uint8_t i = INPUT_INTEGER_DIGITS - integer_fields; i < INPUT_INTEGER_DIGITS; ++i) {
        const bool integer_blink = app_context.input_context.data.input_part ? false : (i == app_context.input_context.data.cursor_position);
        LCD_set_integer(row, pos++, app_context.input_context.data.integer_digits[i], integer_blink);
    }

    if (dot_fields) {
        const unsigned char dot_char = LCD_CHAR_DOT; 
        LCD_set_char(row, pos++, dot_char, false);

        for (uint8_t i = 0; i < fractional_fields; ++i) {
            const bool fractional_blink = app_context.input_context.data.input_part ? (i == app_context.input_context.data.cursor_position) : false;
            LCD_set_integer(row, pos++, app_context.input_context.data.fractional_digits[i], fractional_blink);
        }
    }

    if (measurement_fields) {
        const unsigned char degree_char = LCD_CHAR_DEGREE; 
        LCD_set_char(row, pos++, degree_char, false);
    }

    DEBUG_ASSERT(pos == start_pos + all_angle_fields);
}

// Функции отображения
static inline void input_state_update_display(bool first_time) {
    if (first_time) {
        LCD_clear_line(CONST_INPUT_STATE_MODE_INFO_ROW);

        if (app_context.input_context.mode) {
            LCD_set_string(CONST_INPUT_STATE_MODE_INFO_ROW, 0, "RELATIVE ANGLE:", false);
        }
        else {
            LCD_set_string(CONST_INPUT_STATE_MODE_INFO_ROW, 0, "NEW ANGLE:", false);
        }
    }
    LCD_clear_line(CONST_INPUT_STATE_ACTIVE_ROW);
    input_state_display_angle(CONST_INPUT_STATE_ACTIVE_ROW, true);
    LCD_update_request(false);
}

static inline bool input_state_check_any_digit_set(void) {
    return (app_context.input_context.data.integer_count || app_context.input_context.data.fractional_count);
}

// Функции обновления параметров ввода
static inline void input_state_reset_all_digits(void) {
    memset(&app_context.input_context.data, 0, sizeof(app_context.input_context.data));
    app_context.input_context.data.cursor_position = INPUT_INTEGER_DIGITS - 1;
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
            if (app_context.input_context.data.cursor_position < INPUT_FRACTIONAL_DIGITS - 1 || (app_context.input_context.data.cursor_position == INPUT_FRACTIONAL_DIGITS - 1 && app_context.input_context.data.fractional_digits[INPUT_FRACTIONAL_DIGITS - 1] != 0)) {
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
                app_context.input_context.data.cursor_position = 0;
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
        //if (input_state_check_any_digit_set()) {
            app_context.input_context.data.sign = !app_context.input_context.data.sign;
        //}
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

    for (uint8_t i = INPUT_FRACTIONAL_DIGITS; i < ANGLE_PRECISION; ++i) {
        app_context.input_context.value *= 10;
    }

    if (app_context.input_context.mode) {
        app_context.input_context.value = app_context.input_context.data.sign ? -app_context.input_context.value : app_context.input_context.value;
    }
    else {
        app_context.input_context.value = app_context.input_context.value - app_context.current_angle;
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
        const uint8_t first_digit = INPUT_INTEGER_DIGITS - app_context.input_context.data.integer_count;

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

    if (active_digit < INPUT_FRACTIONAL_DIGITS) {
        if (increment) {
            if (app_context.input_context.data.fractional_digits[active_digit] == 9) {
                //if (active_digit + 1 == app_context.input_context.data.fractional_count) {
                //    app_context.input_context.data.fractional_digits[active_digit] = 1;
                //}
                //else {
                    app_context.input_context.data.fractional_digits[active_digit] = 0;
                //}
            }
            else {
                ++app_context.input_context.data.fractional_digits[active_digit];
            }
        }
        else {
            if (app_context.input_context.data.fractional_digits[active_digit] == 0) { //|| (active_digit + 1 == app_context.input_context.data.fractional_count && app_context.input_context.data.fractional_digits[active_digit] == 1)) {
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
            if (app_context.input_context.data.integer_count) {
                app_context.input_context.data.integer_digits[0] = 0;
                input_state_integer_digits_shift(INPUT_INTEGER_DIGITS - 1, false);
            }
            else {
                input_state_process_cursor_shift(false);
            }
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
    input_state_update_value();
    const bool validation = input_state_validate_input();

    if (validation) {
        if (app_context.input_context.value) {
            app_state_transition_request(APP_STATE_ACTIVE);
        }
        else {
            app_state_transition_request(APP_STATE_IDLE);
        }
    }
    else {
        app_context.input_context.error = true;

        LCD_clear_line(CONST_INPUT_STATE_ACTIVE_ROW);
        LCD_set_string(CONST_INPUT_STATE_ACTIVE_ROW, 0, "INCORRECT INPUT!", true);

        LCD_update_request(false);
        service_timer_enable();
    }
}

static inline void input_state_cancel_input(void) {
    app_state_transition_request(APP_STATE_IDLE);
}

static inline bool inpust_state_check_esc_possible(void) {
    bool result = true;
    if (!input_state_check_any_digit_set()) {
        if (app_context.input_context.data.input_part) {
            result = true;
        }
        else {
            result = false;
        }
    }
    return result;
}

// Обработчик событий ввода
void input_state_event_handler(const event_t *evt) {
    DEBUG_ASSERT(evt);

    if (!app_context.input_context.error) {
        if (evt->id == EVENT_INPUT_ERROR_DISCARD) {
                input_state_reset_all_digits();
                input_state_update_display(false);
        }
        else if (!g_keyboard_block) {
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
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_DOT: {
                            input_state_process_dot();
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_UP: {
                            input_state_process_digit_cyclic_change(true);
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_DOWN: {
                            input_state_process_digit_cyclic_change(false);
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_RIGHT: {
                            input_state_process_cursor_shift(true);
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_LEFT: {
                            input_state_process_cursor_shift(false);
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_SIGN: {
                            input_state_process_sign_change();
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_ESC: {
                            if (inpust_state_check_esc_possible()) {
                                input_state_process_digit_reset();
                                input_state_update_display(false);
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
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_UP: {
                            input_state_process_digit_cyclic_change(true);
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_DOWN: {
                            input_state_process_digit_cyclic_change(false);
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_RIGHT: {
                            input_state_process_cursor_shift(true);
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_LEFT: {
                            input_state_process_cursor_shift(false);
                            input_state_update_display(false);
                            break;
                        }
                        case KEY_ESC: {
                            if (inpust_state_check_esc_possible()) {
                                input_state_process_digit_reset();
                                input_state_update_display(false);
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
        }
    }
}

// Функция обновления подписок
static inline void input_state_manage_subscriptions(bool state) {
    event_bus_t *bus = event_dispatcher_get_bus();

    if (state) {
        event_bus_subscribe(bus, EVENT_KEY_PRESS, input_state_event_handler);
        event_bus_subscribe(bus, EVENT_KEY_REPEAT, input_state_event_handler);
        event_bus_subscribe(bus, EVENT_KEY_RELEASE, input_state_event_handler);
        event_bus_subscribe(bus, EVENT_INPUT_ERROR_DISCARD, input_state_event_handler);
    }
    else {
        event_bus_unsubscribe(bus, EVENT_KEY_PRESS, input_state_event_handler);
        event_bus_unsubscribe(bus, EVENT_KEY_REPEAT, input_state_event_handler);
        event_bus_unsubscribe(bus, EVENT_KEY_RELEASE, input_state_event_handler);
        event_bus_unsubscribe(bus, EVENT_INPUT_ERROR_DISCARD, input_state_event_handler);
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