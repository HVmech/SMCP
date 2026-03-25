#include <globals/LCD_globals.h>

volatile bool g_lcd_blink = false;
volatile bool g_lcd_blink_phase = false;
volatile uint32_t g_lcd_tick = 0;