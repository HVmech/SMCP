#pragma once

#ifndef SERIAL_DEBUG_SERVICE_H
#define SERIAL_DEBUG_SERVICE_H

#include <stdarg.h>
#include <drivers/USART_driver.h>
#include <common/types.h>

bool serial_debug_init(e_USART_port port, uint32_t baudrate, bool remap_pins, bool need_stats); // Инициализация serial

// Базовые функции вывода
void serial_debug_putchar(char ch);
void serial_debug_puts(const char *str);
void serial_debug_putu(uint32_t num);
void serial_debug_puti(int32_t num);

void serial_debug_printf(const char *format, ...); // Форматированный вывод (поддерживает %s, %c, %d, %i, %u, %x, %X, %p, %%)

// Прием данных
bool serial_debug_getchar(uint8_t *ch);

uint16_t serial_debug_available(void);

// Утилиты
void serial_debug_flush(void);
bool serial_debug_is_busy(void);

void serial_debug_echo_simple(void);

#endif // SERIAL_DEBUG_SERVICE_H