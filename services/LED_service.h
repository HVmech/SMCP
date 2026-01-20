#pragma once

#ifndef SMCP_LED_SERVICE_H
#define SMCP_LED_SERVICE_H

#include <drivers/LED_driver.h>

#define LED_DURATION_INFINITE ((uint32_t)-1) // Бесконечное время команды

typedef struct { // Структура команд диода: действие (ВКЛ / ВЫКЛ) и длительность (мс)
    bool action;
    uint32_t duration_ms;
} s_LED_command;

typedef struct { // Конфигурационная структура службы диода: массив команд, длина массива, признак циклического повтора
    const s_LED_command* arr_cmd_seq;
    uint8_t cmd_seq_length;
} s_LED_service_config;

typedef struct { // Контрольная структура службы диода: конфигурация, индекс активной команды, стартовое время (мс), признак активации, диод
    const s_LED_service_config* config;
    uint8_t curr_indx;
    uint32_t start_time_ms;
    bool active;
    bool repeat;
    s_LED_config led;
} s_LED_service;

bool LED_service_init(s_LED_service* instance, const s_LED_service_config* config, e_board_pin pin_code, bool inverted, bool repeat); // Инициализация контрольной структуры
void LED_service_start(s_LED_service* instance); // Запуск службы
void LED_service_stop(s_LED_service* instance); // Остановка службы

bool LED_service_is_running(const s_LED_service* instance); // Проверка состояния службы
void LED_service_update(s_LED_service* instance); // Обновление службы

void LED_service_execute(s_LED_service* instance, const s_LED_service_config* config, bool repeat); // Выполнение новой команды

#endif // SMCP_LED_SERVICE_H
