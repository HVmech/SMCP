#include <common/utils.h>
#include <common/debug_assert.h>

// Внутренние функции:
static inline void memcpy_short(void *dst, const void *src, uint8_t size) { // Копирование памяти побайтово (версия для малых областей)
    DEBUG_ASSERT(dst && src && size != 0); // Проверка валидности входных параметров

    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;

    while (size) {
        *d = *s;
        ++d;
        ++s;
        --size;
    }
}

static inline void memcpy_fixed(void *dst, const void *src, uint16_t size) {
    switch (size) {
        case 1: { *(uint8_t*)dst = *(uint8_t*)src;  break; }
        case 2: { *(uint16_t*)dst = *(uint16_t*)src; break; }
        case 4: { *(uint32_t*)dst = *(uint32_t*)src; break; }
        default: { return; }
    }
}

static inline void memcpy_long(void *dst, const void *src, uint16_t size) { // Копирование памяти побайтово (версия для больших областей)
    DEBUG_ASSERT(dst && src && size != 0); // Проверка валидности входных параметров

    // ... Копирование через DMA (не реализовано)
    DEBUG_ASSERT(size < 255); // Гарантия отсутствия переполнения у функции для малых областей (веременно)
    memcpy_short(dst, src, size); // (временно)
}

// Публичные функции:
void memcpy(void *dst, const void *src, uint16_t size) { // Копирование памяти побайтово
    DEBUG_ASSERT(dst && src && size != 0); // Проверка валидности входных параметров

    if (size < 128) {
        if (size > 4 || size == 3) {
            memcpy_short(dst, src, size); // При небольших размерах областей - простой цикл
        }
        else {
            memcpy_fixed(dst, src, size); // При малых размерах областей - switch/case
        }
    }
    else { 
        memcpy_long(dst, src, size); // При больших размерах областей - DMA
    }
}

void* memset(void* dest, int val, unsigned int count) {
    DEBUG_ASSERT(dest);

    unsigned char* d = (unsigned char*)dest;
    unsigned char v = (unsigned char)val;

    while (count--) { *d = v; ++d; }

    return dest;
}