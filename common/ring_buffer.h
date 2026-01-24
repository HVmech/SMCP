#pragma once

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <common/macro_debug.h>
#include <common/types.h>

typedef struct { // Структура кольцевого буфера для uint8_t
    uint8_t *data;
    uint16_t size;
    volatile uint16_t read;
    volatile uint16_t write;
} s_ring_buffer;

static inline bool ring_buffer_size_is_power_of_two(const s_ring_buffer *rb) { // Проверка на соответствие степени двойки
    DEBUG_STATIC_CHECK_FALSE_RET(rb, false);
    return rb->size > 0 && ((rb->size & (rb->size - 1)) == 0);
}

static inline bool ring_buffer_init(s_ring_buffer *rb, uint8_t *buffer, uint16_t size) { // Инициализация буфера
    DEBUG_STATIC_CHECK_FALSE_RET(rb, false);
    DEBUG_STATIC_CHECK_FALSE_RET(buffer, false);
    DEBUG_STATIC_CHECK_FALSE_RET(size, false);
    rb->data = buffer;
    rb->size = size;
    rb->read = 0;
    rb->write = 0;
    DEBUG_STATIC_CHECK_FALSE_RET(ring_buffer_size_is_power_of_two(rb), false);
    return true;
}

static inline bool ring_buffer_is_empty(const s_ring_buffer *rb) { // Проверка пустоты буфера
    DEBUG_STATIC_CHECK_FALSE_RET(rb, false);
    return rb->write == rb->read;
}

static inline bool ring_buffer_is_full(const s_ring_buffer *rb) { // Проверка заполнения буфера
    DEBUG_STATIC_CHECK_FALSE_RET(rb, false);
    // (uint16_t)(rb->write - rb->read) - разница между количеством чтений и записей
    // (rb->size - 1) - битовая маска с учетом полного заполнения буфера:
    // size == 128 -> (rb->size - 1) == 127 == 0b01111111
    // (uint16_t)~(rb->size - 1) - инверсия битовой маски, старшие разряды - ненулевые, младшие - нулевые
    // (uint16_t)(rb->write - rb->read) & (uint16_t)~(rb->size - 1) - возвращает старшие разряды разницы
    return ((uint16_t)(rb->write - rb->read) & (uint16_t)~(rb->size - 1)) != 0;
}

static inline uint16_t ring_buffer_count(const s_ring_buffer *rb) { // Количество элементов в буфере
    DEBUG_STATIC_CHECK_FALSE_RET(rb, false);
    return (rb->write - rb->read) & (rb->size - 1);
}

static inline bool ring_buffer_write(s_ring_buffer *rb, uint8_t value) { // Запись в буфер
    DEBUG_STATIC_CHECK_FALSE_RET(rb, false);
    if (ring_buffer_is_full(rb)) { return false; }
    rb->data[rb->write & (rb->size - 1)] = value; // Запись данных в головную ячейку
    ++(rb->write); // Сдвиг
    return true;
}

static inline bool ring_buffer_read(s_ring_buffer *rb, uint8_t *value) { // Чтение из буфера
    DEBUG_STATIC_CHECK_FALSE_RET(rb, false);
    DEBUG_STATIC_CHECK_FALSE_RET(value, false);
    if (ring_buffer_is_empty(rb)) { return false; }
    *value = rb->data[rb->read & (rb->size - 1)]; // Копирование данных из хвостовой ячейки
    ++(rb->read); // Сдвиг
    return true;
}

static inline void ring_buffer_clear(s_ring_buffer *rb) { // Очистка буфера
    DEBUG_STATIC_CHECK_FALSE(rb);
    rb->read = 0;
    rb->write = 0;
}

static inline uint8_t ring_buffer_peek_view(const s_ring_buffer *rb, uint16_t index) { // Просмотр элемента по индексу
    DEBUG_STATIC_CHECK_FALSE_RET(rb, false);
    if (index >= ring_buffer_count(rb)) { return 0; }
    return rb->data[(rb->read + index) & (rb->size - 1)];
}

static inline uint8_t ring_buffer_first_view(const s_ring_buffer *rb) { // Просмотр первого элемента
    DEBUG_STATIC_CHECK_FALSE_RET(rb, false);
    return ring_buffer_peek_view(rb, 0);
}

static inline uint8_t ring_buffer_last_view(const s_ring_buffer *rb) { // Просмотр последнего элемента
    DEBUG_STATIC_CHECK_FALSE_RET(rb, false);
    uint16_t cnt = ring_buffer_count(rb);
    if (cnt == 0) { return 0; } 
    return ring_buffer_peek_view(rb, cnt - 1);
}

#endif // RING_BUFFER_H