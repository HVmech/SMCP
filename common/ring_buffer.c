#include <libopencmsis/core_cm3.h>
#include <common/asm.h>
#include <common/utils.h>
#include <common/ring_buffer.h>

void ring_buffer_init(ring_buffer_t *rb, void *buffer, uint16_t size, uint8_t elem_size) {
    DEBUG_ASSERT(rb && buffer);             // Проверка указателей
    DEBUG_ASSERT(size && elem_size);        // Проверка ненулевых размеров
    DEBUG_ASSERT((size & (size - 1)) == 0); // Проверка размера на степень двойки

    rb->write = 0;
    rb->read = 0;
    rb->buffer = (uint8_t *)buffer;
    rb->size = size;
    rb->elem_size = elem_size;
}

bool ring_buffer_push(ring_buffer_t *rb, const void *data) {
    DEBUG_ASSERT(rb && data); // Проверка указателей
    DEBUG_ASSERT(rb->buffer); // Проверка инициализации

    if (ring_buffer_is_full(rb)) { return false; }
    const uint16_t mask = rb->size - 1;
    const uint16_t index = rb->write & mask;
    memcpy(&rb->buffer[index * (uint16_t)rb->elem_size], data, (uint16_t)rb->elem_size);
    MACRO_ASM_DATA_MEMORY_BARRIER; // Запрет на изменение порядка операций и их незавершенность
    rb->write++;
    return true;
}

bool ring_buffer_pop(ring_buffer_t *rb, void *data) {
    DEBUG_ASSERT(rb && data); // Проверка указателей
    DEBUG_ASSERT(rb->buffer); // Проверка инициализации

    if (ring_buffer_is_empty(rb)) { return false; }
    const uint16_t mask = rb->size - 1;
    const uint16_t index = rb->read & mask;
    memcpy(data, &rb->buffer[index * (uint16_t)rb->elem_size], (uint16_t)rb->elem_size);
    MACRO_ASM_DATA_MEMORY_BARRIER; // Запрет на изменение порядка операций и их незавершенность
    rb->read++;
    return true;
}

void ring_buffer_clear(ring_buffer_t *rb) {
    rb->write = 0;
    rb->read = 0;
}