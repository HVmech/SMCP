#pragma once

#ifndef SMCP_MOTION_PANNING_SERVICE_H
#define SMCP_MOTION_PANNING_SERVICE_H

#define DRIVER_PUL_MAX_FREQ_HZ 200000.0 // Потолок частоты для драйвера (DM860A)
#define SYSTEM_PUL_MAX_FREQ_HZ (5.0 * DRIVER_PUL_MAX_FREQ_HZ / 8.0) // Практический потолок частоты (DM860A)
#define MOTOR_FULL_STEPS_PER_REV 200 // Кол-во шагов за оборот
#define DRIVER_MICROSTEP 256 // Делитель драйвера
#define GEARBOX_RATIO  50 // Передаточное число редуктора     
#define STEP_ANGLE_DEG (360.0 / (MOTOR_FULL_STEPS_PER_REV * DRIVER_MICROSTEP * GEARBOX_RATIO)) // Угол за один импульс

#define START_FREQ_HZ 500.0 // Стартовая частота импульсов
#define MAX_ACCEL_HZ_S 5000.0 // Максимально допустимое ускорение

#define STEPPER_ISR_HZ 10000U // Частота настроечного таймера

#include <common/types.h>

extern const uint32_t f_update_max;
extern const uint32_t f_clk;
extern const uint32_t pwm_prescaler;

extern const uint32_t v_min;
extern const uint32_t v_max;
extern const uint32_t a_max;
extern const uint32_t j_max;

extern const uint32_t f_tim; // Частота обновления таймера

extern const int32_t f_max; // Максимальная частота импульсов
extern const int32_t f_min; // Минимальная частота импульсов
extern const int32_t df_max; // Максимальное изменение скорости
extern const int32_t ddf_max; // Максимальное изменение ускорения

extern const uint32_t repetitions; // Количество обновлений для генерации прерывания

typedef enum {
    PHASE_J1 = 0,
    PHASE_A1,
    PHASE_J2,
    PHASE_C,
    PHASE_J3,
    PHASE_A2,
    PHASE_J4,
    PHASE_TAIL,
    PHASE_COUNT
} motion_phase_type_t;

typedef struct {
    int32_t f0;
    int32_t df0;
    int32_t ddf0;
    uint32_t update_steps;
} motion_phase_t;

typedef struct {
    motion_phase_t motion_phases[PHASE_COUNT];
    bool tail_phase;
    bool direction;
} motion_block_t;

motion_block_t plan_motion(uint32_t total_steps, bool direction);

#endif // SMCP_MOTION_PANNING_SERVICE_H