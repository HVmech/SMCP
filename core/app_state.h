#pragma once
#ifndef SMCP_APP_STATE_H
#define SMCP_APP_STATE_H

#include <core/event_bus.h>
#include <common/types.h>

typedef enum { // Перечислитель состояний
    APP_STATE_IDLE = 0,
    APP_STATE_INPUT,
    APP_STATE_ACTIVE,
    APP_STATE_ERROR
} app_state_code_t;

typedef struct { // Структура текущего состояния
    void_func_ptr_t enter;
    void_func_ptr_t exit;
    event_handler_t event_handler;
} app_state_t;

void app_state_transition_request(app_state_code_t state); // Функция запроса смены текущего состояния

#endif // SMCP_APP_STATE_H
