#include <services/LCD_service.h>

#include <globals/LCD_globals.h>

#include <services/motion_controller_service.h>
#include <services/debug_serial_service.h>

#include <core/app_context.h>
#include <core/input_data.h>
#include <core/service_timer.h>
#include <core/event_dispatcher.h>
#include <core/event_bus.h>

#include <drivers/time_driver.h>

#include <common/types.h>

typedef struct {
    uint8_t ch;
    bool blink;
} LCD_field_t;

typedef struct {
    LCD_field_t fields[LCD_LENGTH * LCD_HEIGHT];
} LCD_buffer_t;

typedef struct {
    LCD_buffer_t current;
    LCD_buffer_t previous;
    bool change[LCD_LENGTH * LCD_HEIGHT];
} LCD_controller_t;

static LCD_controller_t lcd = {0};

void LCD_set_char(uint8_t row, uint8_t column, uint8_t ch, bool blink) {
    DEBUG_ASSERT(row < LCD_HEIGHT && column < LCD_LENGTH);

    lcd.current.fields[LCD_LENGTH * row + column].ch = ch;
    lcd.current.fields[LCD_LENGTH * row + column].blink = blink;
}

void LCD_set_string(uint8_t row, uint8_t column, const char* str, bool blink) {
    for (uint8_t i = 0; str[i] && column + i < LCD_LENGTH; ++i) {
        LCD_set_char(row, column + i, (uint8_t)str[i], blink);
    }
}

void LCD_set_integer(uint8_t row, uint8_t column, uint32_t value, bool blink) {
    // TODO: переделать с прямым порядком
    uint8_t digit = 0;
    do {
        digit = value % 10;
        LCD_set_char(row, column++, LCD_CHAR_NULL + digit, blink);
        value /= 10;
    } while (value && column < LCD_LENGTH);
}

void LCD_clear_line(uint8_t row) {
    for (uint8_t i = 0; i < LCD_LENGTH; ++i) {
        LCD_set_char(row, i, LCD_CHAR_EOF, false);
    }
}

void LCD_clear_display(void) {
    for (uint8_t r = 0; r < LCD_HEIGHT; ++r) {
        LCD_clear_line(r);
    }
}

void LCD_update_request(bool from_isr) {
    bool any_blink = false;
    for (uint8_t i = 0; i < LCD_LENGTH * LCD_HEIGHT; ++i) {
        lcd.change[i] = (lcd.current.fields[i].ch != lcd.previous.fields[i].ch || lcd.current.fields[i].blink != lcd.previous.fields[i].blink);
        any_blink = any_blink || lcd.current.fields[i].blink;
    }

    if (g_lcd_blink != any_blink && any_blink == true) {
        g_lcd_blink = any_blink;
        if (any_blink) { service_timer_enable(); }
    }

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

void LCD_test_helper_handler(const event_t *evt_inp) {
    static bool prev_blink_phase = false;
    
    switch (evt_inp->id) {
        case EVENT_LCD_UPDATE_REQUEST: {
            bool blink_phase_has_changed = prev_blink_phase != g_lcd_blink_phase;
            prev_blink_phase = g_lcd_blink_phase;

            for (uint8_t i = 0; i < LCD_LENGTH * LCD_HEIGHT; ++i) {
                if (lcd.change[i] || (lcd.current.fields[i].blink && blink_phase_has_changed)) {
                    const uint8_t ch = lcd.current.fields[i].blink ? (g_lcd_blink_phase ? lcd.current.fields[i].ch : LCD_CHAR_EOF) : lcd.current.fields[i].ch;
                    debug_serial_putchar(ch);
                    
                    if (i == LCD_LENGTH - 1 || i == LCD_LENGTH * LCD_HEIGHT - 1) {
                        debug_serial_putchar('\n');
                    }

                    lcd.previous.fields[i].blink = lcd.current.fields[i].blink;
                    lcd.previous.fields[i].ch = lcd.current.fields[i].ch;
                }
            }
            break;
        }
        default: { break; }
    }
}