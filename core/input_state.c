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
            const uint32_t duration = evt->payload.data.unsigned_value & (~((1 << 8) - 1));

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

const app_state_t input_state = {
    .enter = input_state_enter,
    .exit = input_state_exit,
    .event_handler = input_state_event_handler
};
