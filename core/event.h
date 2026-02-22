#pragma once
#ifndef SMCP_EVENT_H
#define SMCP_EVENT_H

#include <common/types.h>

typedef enum { // Перечислитель событий
    EVENT_NONE = 0,
    EVENT_PRINT_STATS,
    EVENT_USART1_RX,
    EVENT_USART2_RX,
    EVENT_USART3_RX,
    EVENT_LED_SERVICE_UPDATE,
    EVENT_LED_CONTROL,
    //...
    EVENT_MOTOR_ROTATION_REQUEST,
    EVENT_MOTOR_ROTATION_TEST_REQUEST,
    EVENT_MOTOR_ROTATION_PREPARE,
    EVENT_MOTOR_PREPARATION_COMPLETE,
    EVENT_MOTOR_ROTATION_START,
    EVENT_MOTOR_ROTATION_COMPLETE,
    EVENT_MOTOR_EMERGENCY_STOP,
    EVENT_TYPES_NUM
} event_id_t;

typedef enum { // Перечислитель приоритетов
    EVENT_PRIORITY_HIGH = 0,
    EVENT_PRIORITY_NORMAL,
    EVENT_PRIORITY_LOW,
    EVENT_PRIORITY_NUM
} event_priority_t;

typedef enum { // Перечислитель флагов событий
    EVENT_FLAG_NONE = 0,
    EVENT_FLAG_DEDUPLICATE_LAST = (1U << 0)
} event_flags_t;

typedef enum { // Перечислитель типа переносимых данных
    EVENT_DATA_NONE = 0,
    EVENT_DATA_UNSIGNED,
    EVENT_DATA_SIGNED,
} event_data_type_t;

typedef union { // Объединения для хранения переносимых данных
    uint32_t unsigned_value;
    int32_t  signed_value;
} event_data_t;

typedef struct { // Структура переносимых данных
    event_data_type_t type;
    event_data_t data;
} event_payload_t;

typedef struct { // Структура события
    event_id_t id;
    event_priority_t priority;
    event_flags_t flags;
    event_payload_t payload;
    uint32_t timestamp;
} event_t;

#endif // SMCP_EVENT_H
