#pragma once
#ifndef SMCP_LCD_GLOBALS_H
#define SMCP_LCD_GLOBALS_H

#include <common/types.h>

extern volatile bool g_lcd_blink;
extern volatile bool g_lcd_blink_phase;
extern volatile uint32_t g_lcd_tick;

#endif // SMCP_LCD_GLOBALS_H