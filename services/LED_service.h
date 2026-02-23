#pragma once

#ifndef SMCP_LED_SERVICE_H
#define SMCP_LED_SERVICE_H

#include <core/event.h>
#include <drivers/LED_driver.h>

#define CONST_LED_SERVICE_MAX_LEDS 4 // Максимальное количество управляемых диодов

typedef enum {
    LED_BUILTIN = 0,
    LED_MOTOR_STOP_BUTTON
} LED_id_t;

typedef struct { // Структура команд диода: действие (ВКЛ / ВЫКЛ) и длительность (мс)
    bool action;
    uint32_t duration_ms;
} LED_command_t;

typedef struct { // Конфигурационная структура службы диода
    const LED_command_t* arr_cmd_seq;
    uint8_t cmd_seq_length;
} LED_pattern_t;

typedef struct { // Структура конфигурации командного паттерна
    const LED_pattern_t* exec_pattern;
    uint32_t last_change_ms;
    uint8_t curr_indx;
    bool repeat;
} LED_exec_config_t;

typedef struct { // Структура службы диодов
    LED_t led[CONST_LED_SERVICE_MAX_LEDS];
    LED_exec_config_t config[CONST_LED_SERVICE_MAX_LEDS];
    uint8_t led_num;
} LED_service_t;

// Основные функции
bool LED_service_init_led(uint8_t led_id, board_pin_e pin_code, bool inverted); // Инициализация
void LED_service_execute(uint8_t led_id, const LED_pattern_t* pattern, bool repeat); // Запуск командного паттерна
void LED_service_set_led_state(uint8_t led_id, bool state); // Установка состояния диода
void LED_service_update(void); // Обновление конфигурации командных паттернов
void LED_service_handle_event(const event_t *evt); // Обработчик события управления диодом

#endif // SMCP_LED_SERVICE_H
