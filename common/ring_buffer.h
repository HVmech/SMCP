#include <common/types.h>
#include <common/debug_assert.h>

#ifndef SMCP_RING_BUFFER_H
#define SMCP_RING_BUFFER_H

// Кольцевой буфер для соглашения SPSC для main <---> ISR

typedef struct {
    volatile uint16_t write; // Счетчик записей
    volatile uint16_t read;  // Счетчик чтений

    uint8_t  *buffer;   // Ссылка на буфер
    uint8_t elem_size;  // Размер одного элемента в байтах
    uint16_t size;      // Количество элементов (степень числа 2)
} ring_buffer_t;

// Вспомогательные функции:
static inline bool ring_buffer_is_empty(const ring_buffer_t *rb) {
    DEBUG_ASSERT(rb); // Проверка указателей
    DEBUG_ASSERT(rb->buffer); // Проверка инициализации

    return rb->write == rb->read;; // Проверка пустоты
}

static inline bool ring_buffer_is_full(const ring_buffer_t *rb) {
    DEBUG_ASSERT(rb); // Проверка указателей
    DEBUG_ASSERT(rb->buffer); // Проверка инициализации

    const uint16_t mask = ~(rb->size - 1);
    return ((rb->write - rb->read) & mask) != 0; // Проверка полноты
}

// Основные функции:
void ring_buffer_init(ring_buffer_t *rb, void *buffer, uint16_t size, uint8_t elem_size);
bool ring_buffer_push(ring_buffer_t *rb, const void *data);
bool ring_buffer_pop(ring_buffer_t *rb, void *data);
void ring_buffer_clear(ring_buffer_t *rb);

// Вспомогательные функции:
bool ring_buffer_peek_last(const ring_buffer_t *rb, void *out); // Просмотр последнего элемента

#endif // SMCP_RING_BUFFER_H