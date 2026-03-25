#include <services/LCD_service.h>

#include <globals/LCD_globals.h>

#include <services/motion_controller_service.h>
#include <services/debug_serial_service.h>

#include <core/app_context.h>
#include <core/input_data.h>
#include <core/service_timer.h>
#include <core/event_dispatcher.h>
#include <core/event_bus.h>

#include <drivers/LCD_control_timer_driver.h>
#include <drivers/LCD_driver.h>
#include <drivers/time_driver.h>

#include <common/utils.h>
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
    const uint8_t digits_count = MAX_VALUE(digcnt(value), 1);
    uint8_t digit = 0;
    uint8_t col = column + digits_count;

    do {
        --col;

        if (col < LCD_LENGTH) {
            digit = value % 10;
            LCD_set_char(row, col, LCD_CHAR_NULL + digit, blink);
        }
        value /= 10;

    } while (col > column);
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

    if (g_lcd_blink != any_blink) {
        if (any_blink == true) {
            g_lcd_blink = true;
            service_timer_enable();
        }
        else {
            g_lcd_blink = false;
        }
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

void LCD_handler(const event_t *evt_inp) {
    static bool prev_blink_phase = false;
    static bool need_shift = true;
    
    switch (evt_inp->id) {
        case EVENT_LCD_UPDATE_REQUEST: {
            bool blink_phase_has_changed = prev_blink_phase != g_lcd_blink_phase;
            prev_blink_phase = g_lcd_blink_phase;

            // Проходим по всем строкам
            for (uint8_t row = 0; row < LCD_HEIGHT; ++row) {
                // Проходим по всем столбцам в текущей строке
                for (uint8_t col = 0; col < LCD_LENGTH; ++col) {
                    uint8_t index = row * LCD_LENGTH + col;
                    
                    // Проверяем, нужно ли обновлять этот символ
                    bool need_update = lcd.change[index] || (lcd.current.fields[index].blink && blink_phase_has_changed);
                    
                    if (need_update) {
                        // Сохраняем предыдущее состояние
                        lcd.previous.fields[index].blink = lcd.current.fields[index].blink;
                        lcd.previous.fields[index].ch = lcd.current.fields[index].ch;

                        if (!LCD_control_timer_is_running()) { LCD_control_timer_enable(); }
                        
                        // Устанавливаем курсор только если нужно
                        if (need_shift) {
                            if (row == 0) {
                                lcd_write_byte(0x80 + col, 0);  // 1-я строка
                            } else {
                                lcd_write_byte(0xC0 + col, 0);  // 2-я строка
                            }
                            need_shift = false;
                        }
                        
                        // Определяем символ для вывода
                        uint8_t ch = (!lcd.current.fields[index].blink || g_lcd_blink_phase) ? lcd.current.fields[index].ch : LCD_CHAR_EOF;
                        
                        if (ch == LCD_CHAR_EOF || ch == 0) { 
                            ch = ' '; 
                        }
                        
                        lcd_write_byte(ch, 1);
                        
                        // Сбрасываем флаг изменения
                        lcd.change[index] = false;
                    }
                    else {
                        // Если символ не изменился, то при следующем изменившемся символе
                        // нужно будет установить курсор, так как позиция изменилась
                        need_shift = true;
                    }
                }
                // при переходе на новую строку нужно сбросить need_shift в true
                need_shift = true;
            }
            break;
        }
        default: { break; }
    }
}
/*
void LCD_handler(const event_t *evt_inp) {
    static bool prev_blink_phase = false;
    static bool prev_changed = false;
    
    switch (evt_inp->id) {
        case EVENT_LCD_UPDATE_REQUEST: {
            bool blink_phase_has_changed = prev_blink_phase != g_lcd_blink_phase;
            prev_blink_phase = g_lcd_blink_phase;

            lcd_cursor_home();
            prev_changed = false;

            for (uint8_t i = 0; i < LCD_LENGTH * LCD_HEIGHT; ++i) {
                
                if (lcd.change[i] || (lcd.current.fields[i].blink && blink_phase_has_changed)) {
                    lcd.previous.fields[i].blink = lcd.current.fields[i].blink;
                    lcd.previous.fields[i].ch = lcd.current.fields[i].ch;
                    prev_changed = true;
                    if (!prev_changed) { lcd_set_cursor_place(i / LCD_LENGTH + 1, i % LCD_LENGTH + 1); }
                    uint8_t ch = lcd.current.fields[i].blink ? (g_lcd_blink_phase ? lcd.current.fields[i].ch : LCD_CHAR_EOF) : lcd.current.fields[i].ch;
                    if (ch == LCD_CHAR_EOF) { ch = '_'; }
                    if (i == LCD_LENGTH - 1 ) {
                        lcd_set_cursor_place(2, 1);
                    }
                    
                    lcd_write_byte(ch, 1);
                }
                else {
                    prev_changed = false;
                }
            }
            break;
        }
        default: { break; }
    }
}
*/
void LCD_test_helper_handler(const event_t *evt_inp) {
    static bool prev_blink_phase = false;
    
    switch (evt_inp->id) {
        case EVENT_LCD_UPDATE_REQUEST: {
            bool blink_phase_has_changed = prev_blink_phase != g_lcd_blink_phase;
            prev_blink_phase = g_lcd_blink_phase;

            for (uint8_t i = 0; i < LCD_LENGTH * LCD_HEIGHT; ++i) {
                if (lcd.change[i] || (lcd.current.fields[i].blink && blink_phase_has_changed)) {
                    lcd.previous.fields[i].blink = lcd.current.fields[i].blink;
                    lcd.previous.fields[i].ch = lcd.current.fields[i].ch;
                }

                uint8_t ch = lcd.current.fields[i].blink ? (g_lcd_blink_phase ? lcd.current.fields[i].ch : LCD_CHAR_EOF) : lcd.current.fields[i].ch;
                switch (ch) {
                    case LCD_CHAR_DOT: {
                        ch = '.';
                        break;
                    }
                    case LCD_CHAR_EOF: {
                        ch = '_';
                        break;
                    }
                    case LCD_CHAR_DEGREE: {
                        ch = '*';
                        break;
                    }
                    case LCD_CHAR_PLUS: {
                        ch = '+';
                        break;
                    }
                    case LCD_CHAR_MINUS: {
                        ch = '-';
                        break;
                    }
                    default: {
                        break;
                    }
                }

                debug_serial_putchar(ch);
                
                if (i == LCD_LENGTH - 1 || i == LCD_LENGTH * LCD_HEIGHT - 1) {
                    debug_serial_putchar('\n');
                }
            }
            break;
        }
        default: { break; }
    }
}

void LCD_delay_block(uint32_t delay) {
    delay_ms(delay);
    lcd_delay_stop();
}

void LCD_init(void) {
    event_bus_t *bus = event_dispatcher_get_bus();
    event_bus_subscribe(bus, EVENT_LCD_UPDATE_REQUEST, LCD_handler);
    event_bus_subscribe(bus, EVENT_LCD_UPDATE_REQUEST, LCD_test_helper_handler);

    memset(&lcd, 0, sizeof(LCD_controller_t));

    lcd_init(LCD_control_timer_async_delay_us);
    //lcd_init(LCD_delay_block);
    LCD_control_timer_init(1, 7, lcd_process, lcd_delay_stop);
    lcd_init_display();
    delay_ms(1000);

    LCD_control_timer_enable();
}