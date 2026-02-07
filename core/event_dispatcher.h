#pragma once

#ifndef SMCP_EVENT_DISPATCHER_H
#define SMCP_EVENT_DISPATCHER_H

#include <core/event_bus.h>

typedef struct { // Структура диспетчера событий
    event_bus_t *bus; // Связанная шина событий
} event_dispatcher_t;

void event_dispatcher_init(event_bus_t *bus); // Инициализация
event_bus_t* event_dispatcher_get_bus(); // Возврат шины
bool event_dispatcher_process(); // Обработка события с наивысшим доступным приоритетом

#endif // SMCP_EVENT_DISPATCHER_H
