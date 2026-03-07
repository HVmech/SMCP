#pragma once

#ifndef SMCP_UTILS_H
#define SMCP_UTILS_H

#include <common/types.h>

void memcpy(void *dst, const void *src, uint16_t size); // Копирование памяти побайтово
void* memset(void* dest, int val, unsigned int count); // Заполнение памяти побайтово

#endif // SMCP_UTILS_H