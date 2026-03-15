#include <services/LCD_service.h>

#include <services/motion_controller_service.h>
#include <services/debug_serial_service.h>

#include <core/app_context.h>
#include <core/input_data.h>
#include <core/event_dispatcher.h>
#include <core/event_bus.h>
#include <core/event.h>

#include <drivers/time_driver.h>
#include <stdint.h>

#define LCD_LENGTH 16
#define LCD_HEIGHT 2

#define LCD_CHAR_EOF 0x0
#define LCD_CHAR_PLUS 0x2B
#define LCD_CHAR_MINUS 0x2D
#define LCD_CHAR_DOT 0x2E
#define LCD_CHAR_NULL 0x30
#define LCD_CHAR_DEGREE 0xDF

typedef struct {
    char ch;
    bool blink;
} LCD_field_t;

typedef struct {
    uint8_t pos;
    bool visible;
} LCD_cursor_t;

typedef struct {
    LCD_field_t fields[LCD_LENGTH * LCD_HEIGHT];
    LCD_cursor_t cursor;
} LCD_buffer_t;

static LCD_buffer_t lcd = {0};

static uint8_t LCD_format_angle(char *buf, int32_t value, bool show_sign) {
    // Определение знака
    bool sign = false;
    if (value < 0) {
        sign = true;
        value = -value;
    }

    // Определение целой и дробной части
    int32_t precision = 1;
    for (uint8_t i = 0; i < INPUT_FRACTIONAL_DIGITS; i++) { precision *= 10; }

    uint32_t integer_value  = value / precision;
    uint32_t fractional_value = value % precision;
    uint8_t len = 0;

    buf[len++] = LCD_CHAR_DEGREE;

    // Дробная часть
    uint8_t fractional_digit_count = INPUT_FRACTIONAL_DIGITS;
    while (fractional_digit_count > 0 && (fractional_value % 10) == 0) { // Удаление замыкающих нулей
        fractional_value /= 10;
        --fractional_digit_count;
    }

    if (fractional_digit_count > 0) { // Вывод дробных разрядов
        for (uint8_t i = 0; i < fractional_digit_count; i++) {
            buf[len++] = LCD_CHAR_NULL + (fractional_value % 10);
            fractional_value /= 10;
        }
        buf[len++] = LCD_CHAR_DOT; // Вывод точки только в случае наличия дробной части
    }

    // Целая часть
    if (integer_value == 0) { // Если целых нет
        buf[len++] = LCD_CHAR_NULL;
    }
    else {
        while (integer_value > 0) {
            buf[len++] = LCD_CHAR_NULL + (integer_value % 10);
            integer_value /= 10;
        }
    }

    // Вывод знака
    if (show_sign) {
        if (sign) {
            buf[len++] = LCD_CHAR_MINUS;
        }
        else {
            buf[len++] = LCD_CHAR_PLUS;
        }
    }

    buf[len++] = LCD_CHAR_EOF;

    return len;
}

void LCD_set_char(uint8_t row, uint8_t column, char ch, bool blink) {
    lcd.fields[LCD_LENGTH * row + column].ch = ch;
    lcd.fields[LCD_LENGTH * row + column].blink = blink;
}

void LCD_set_cursor(uint8_t row, uint8_t column, bool visible) {
    lcd.cursor.pos = LCD_LENGTH * row + column;
    lcd.cursor.visible = visible;
}

void LCD_set_string(uint8_t row, uint8_t column, const char* str, bool blink, bool reversed) {
    int8_t col = column;

    if (reversed) {
        for (uint8_t i = 0; str[i] && col >= 0; i++) {
            LCD_set_char(row, col, str[i], blink);
            --col;
        }
    }
    else {
        for (int8_t i = 0; str[i] && col < LCD_LENGTH; ++i) {
            LCD_set_char(row, col, str[i], blink);
            ++col;
        }
    }
}

void LCD_set_integer(uint8_t row, uint8_t column, uint32_t value, bool blink) {
    uint8_t digit = 0;
    while (value && column < LCD_LENGTH) {
        digit = value % 10;
        LCD_set_char(row, column++, '0' + digit, blink);
        value /= 10;
    }
}

void LCD_clear_line(uint8_t row) {
    for (uint8_t i = 0; i < LCD_LENGTH; ++i) {
        LCD_set_char(row, i, 0, false);
    }
}

void LCD_display_angle(uint8_t row, uint8_t column, int32_t angle, bool show_sign, bool blink) {
    char buf[LCD_LENGTH];
    LCD_format_angle(buf, angle, show_sign);
    LCD_set_string(row, column, buf, blink, true);
}

void LCD_update_request(bool from_isr) {
    event_bus_t *bus = event_dispatcher_get_bus();

    event_t evt = {0};
    evt.id = EVENT_LCD_UPDATE_REQUEST;
    evt.priority = EVENT_PRIORITY_LOW;
    evt.flags = EVENT_FLAG_DEDUPLICATE_LAST;
    evt.timestamp = get_current_time_ms();

    if (from_isr) {
        event_bus_post_from_isr(bus, &evt);
    }
    else {
        event_bus_post(bus, &evt);
    }
}