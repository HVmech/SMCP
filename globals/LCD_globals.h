#pragma once
#ifndef SMCP_LCD_GLOBALS_H
#define SMCP_LCD_GLOBALS_H

#include <common/types.h>

extern volatile bool g_lcd_blink; // Признак включения режима мигания
extern volatile bool g_lcd_blink_phase; // Текущая фаза мигания
extern volatile uint32_t g_lcd_tick; // Временная метка

#endif // SMCP_LCD_GLOBALS_H