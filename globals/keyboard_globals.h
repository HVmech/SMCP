#pragma once
#ifndef SMCP_KEYBOARD_GLOBALS_H
#define SMCP_KEYBOARD_GLOBALS_H

#include <common/types.h>

extern volatile bool g_keyboard_block;
extern volatile uint32_t g_keyboard_block_time;

#endif // SMCP_KEYBOARD_GLOBALS_H