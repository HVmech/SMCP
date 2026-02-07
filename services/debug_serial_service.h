#pragma once

#ifndef SMCP_DEBUG_SERIAL_SERVICE_H
#define SMCP_DEBUG_SERIAL_SERVICE_H

#include <stdarg.h>
#include <core/event.h>
#include <drivers/USART_driver.h>
#include <common/types.h>

bool debug_serial_init(USART_port_e port, uint32_t baudrate, bool remap, bool enable_stats); // Инициализация serial

// Базовые функции вывода
void debug_serial_putchar(char ch);
void debug_serial_puts(const char *str);
void debug_serial_putu(uint32_t num);
void debug_serial_puti(int32_t num);

void debug_serial_printf(const char *format, ...); // Форматированный вывод (поддерживает %s, %c, %d, %i, %u, %x, %X, %p, %%)

// Прием данных
bool debug_serial_getchar(uint8_t *ch);

// Утилиты
//void debug_serial_echo_simple(void);
void serial_print_stats(void);

// События
void debug_serial_handle_event(const event_t *evt);

#endif // SMCP_DEBUG_SERIAL_SERVICE_H