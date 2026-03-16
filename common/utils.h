#pragma once

#ifndef SMCP_UTILS_H
#define SMCP_UTILS_H

#include <common/types.h>

#define MAX_VALUE(var1, var2) ((var2) > (var1) ? (var2) : (var1))
#define MIN_VALUE(var1, var2) ((var2) < (var1) ? (var2) : (var1))

void memcpy(void *dst, const void *src, uint16_t size); // Копирование памяти побайтово
void* memset(void* dest, int val, unsigned int count); // Заполнение памяти побайтово
uint8_t digcnt(uint32_t value); // Количество разрядов в числе, если число - 0, то возврат 0
uint8_t trzercnt(uint32_t value, uint8_t digit_count); // Количество замыкающих нулей числа

#endif // SMCP_UTILS_H