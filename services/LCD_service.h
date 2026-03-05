#pragma once

#ifndef SMCP_LCD_SERVICE_H
#define SMCP_LCD_SERVICE_H

#include <common/types.h>

void LCD_set_char(uint8_t row, uint8_t column, char ch, bool blink);
void LCD_set_blink(uint8_t row, uint8_t column, bool state);
void LCD_set_string(uint8_t row, uint8_t column, const char* str, bool blink);
void LCD_clear_line(uint8_t row);
void LCD_display_angle(uint8_t row, uint8_t column, int32_t angle);

#endif // SMCP_LCD_SERVICE_H