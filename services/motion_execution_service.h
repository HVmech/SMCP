#pragma once
#ifndef SMCP_MOTION_EXECUTION_SERVICE_H
#define SMCP_MOTION_EXECUTION_SERVICE_H

#include <services/motion_planning_service.h>
#include <common/board.h>
#include <common/types.h>

typedef struct { // Структура конфигурации исполнителя
    motion_block_t current_block; // Текущий блок движения
    
    // Текущее состояние
    uint8_t current_phase;
    uint32_t phase_updates_left;
    uint32_t phase_updates_made;
    
    // Динамические параметры (интегрируются)
    int32_t f;
    int32_t df;
    int32_t ddf;
    
    // Флаги
    bool is_running;
    bool is_initialized;
} motion_executor_config_t;

// Публичные функции
void motion_executor_init(board_pin_e dir_pin, board_pin_e ena_pin);
void motion_executor_start(motion_block_t* block);
void motion_executor_stop(void);
//bool motion_executor_is_running(void);
//uint32_t motion_executor_get_current_speed(void);
void motion_executor_notify(bool state);
void motion_executor_telemetry_update(void);
uint32_t motion_executor_get_position_change_in_steps(void);
void motion_executor_recovery(void);

#endif // SMCP_MOTION_EXECUTION_SERVICE