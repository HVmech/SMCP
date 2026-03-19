#pragma once
#ifndef SMCP_LCD_SERVICE_H
#define SMCP_LCD_SERVICE_H

#include <core/event.h>
#include <common/types.h>

#define LCD_LENGTH 16
#define LCD_HEIGHT 2

#define LCD_CHAR_EOF 0x0
#define LCD_CHAR_PLUS 0x2B
#define LCD_CHAR_MINUS 0x2D
#define LCD_CHAR_DOT 0x2E
#define LCD_CHAR_NULL 0x30
#define LCD_CHAR_DEGREE 0xDF

#define CONST_LCD_BLINK_INTERVAL 400

void LCD_set_char(uint8_t row, uint8_t column, unsigned char ch, bool blink);
void LCD_set_cursor(uint8_t row, uint8_t column, bool visible);
void LCD_set_string(uint8_t row, uint8_t column, const char* str, bool blink);
void LCD_set_integer(uint8_t row, uint8_t column, uint32_t value, bool blink);
void LCD_clear_line(uint8_t row);
void LCD_clear_display(void);

void LCD_update_request(bool from_isr);

void LCD_test_helper_handler(const event_t *evt_inp);

#endif // SMCP_LCD_SERVICE_H