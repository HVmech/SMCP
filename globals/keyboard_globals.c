#include <globals/keyboard_globals.h>

volatile bool g_keyboard_block = false;
volatile uint32_t g_keyboard_block_time = 0;