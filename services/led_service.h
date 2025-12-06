// services/led_service.h
#pragma once
#include <common/types.h>

void led_init(void);
void led_set(bool state);
void led_toggle(void);
void led_blink(uint32_t freq_hz);