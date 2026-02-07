#pragma once

#ifndef SMCP_EVENT_BUS_H
#define SMCP_EVENT_BUS_H

#include <core/event.h>

#include <common/ring_buffer.h>
#include <common/types.h>

#define EVENT_BUS_MAX_SUBSCRIBERS 6

typedef void (*event_handler_t)(const event_t *evt);

typedef struct { // Структура статистики шины событий
    uint16_t dropped;          // Не добавлено из-за переполнения
    uint16_t deduplicated;     // Не добавлено из-за дедупликации
    uint16_t posted_from_isr;  // Добавлено из ISR
    uint16_t posted_from_main; // Добавлено из main
    uint16_t invoked;          // Обработано
} event_bus_stats_t;

typedef struct { // Структура подписки на событие
    event_handler_t handler[EVENT_BUS_MAX_SUBSCRIBERS]; // Обработчики подписчиков
    uint8_t handler_count; // Количество подписчиков на каждый тип события
} event_subscription_t;

typedef struct { // Структура шины событий
    ring_buffer_t *queues[EVENT_PRIORITY_NUM]; // Очереди по приоритетам
    event_subscription_t subscriptions[EVENT_TYPES_NUM]; // Таблица подписок
    event_bus_stats_t stats[EVENT_PRIORITY_NUM]; // Статистика
} event_bus_t;

// Основные функции:
void event_bus_init(event_bus_t *bus, ring_buffer_t *queues[EVENT_PRIORITY_NUM]); // Инициализация
bool event_bus_post(event_bus_t *bus, const event_t *evt); // Публикация события из основного потока
bool event_bus_post_from_isr(event_bus_t *bus, const event_t *evt); // Публикация события из прерывания
bool event_bus_subscribe(event_bus_t *bus, event_id_t id, event_handler_t handler); // Подписка
bool event_bus_unsubscribe(event_bus_t *bus, event_id_t id, event_handler_t handler); // Отписка

// Вспомогательные функции:
void event_bus_invoke(event_bus_t *bus, const event_t *evt); // Вызов обработчиков для события
void event_bus_clear(event_bus_t *bus); // Очистка

#endif // SMCP_EVENT_BUS_H
