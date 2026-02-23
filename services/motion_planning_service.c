#include <services/motion_planning_service.h>
#include <services/debug_serial_service.h>
#include <common/debug_assert.h>

const uint32_t f_update = 10000;
const uint32_t f_clk = 72000000;
const uint32_t pwm_prescaler = 36;

const uint32_t v_min = 1000;
const uint32_t v_max = 150000;
const uint32_t a_max = 5000;
const uint32_t j_max = 1;

const uint32_t f_tim = f_clk / pwm_prescaler; // Частота обновления таймера

const int32_t f_max = v_max; // Максимальная частота импульсов
const int32_t f_min = v_min; // Минимальная частота импульсов
const int32_t df_max = a_max; // Максимальное изменение скорости
const int32_t ddf_max = j_max; // Максимальное изменение ускорения

const uint32_t repetitions_calc = f_tim / f_update; // Количество обновлений для генерации прерывания
const uint32_t repetitions = repetitions_calc > 255 ? 255 : (repetitions_calc < 10 ? 10 : repetitions_calc);

motion_block_t plan_motion(uint32_t total_steps, bool direction) {
    const uint32_t total_updates = total_steps / repetitions;
    const uint32_t tail_phase_steps = total_steps % repetitions;

    debug_serial_printf("f_max = %d, f_min = %d\r\n", f_max, f_min);
    debug_serial_printf("df_max = %d, ddf_max = %d\r\n", df_max, ddf_max);
    debug_serial_printf("repetitions = %d, total_updates = %d, tail_phase_steps = %d\r\n", repetitions, total_updates, tail_phase_steps);

    motion_block_t result; // Объявление переменной, в которую записывается результат расчета
    for (uint8_t i = 0; i < PHASE_COUNT; ++i) {
        result.motion_phases[i].ddf0 = 0;
        result.motion_phases[i].df0 = 0;
        result.motion_phases[i].f0 = f_min;
        result.motion_phases[i].update_steps = 0;
    } 
    result.tail_phase = false;
    result.direction = direction;

    // Шаги в фазах J, A, C:
    uint32_t Nj = 0;
    uint32_t Na = 0;
    uint32_t Nc = 0;

    // 0. Определение наличия остаточных шагов
    if (tail_phase_steps > 0) {
        result.tail_phase = true;
        result.motion_phases[PHASE_TAIL].f0 = f_min;
        result.motion_phases[PHASE_TAIL].df0 = 0;
        result.motion_phases[PHASE_TAIL].ddf0 = 0;
        result.motion_phases[PHASE_TAIL].update_steps = tail_phase_steps;
    }
    if (total_updates == 0) { return result; } // Определение вырожденного случая total_updates == 0

    // 1. Расчет фазы J1: Движение с нарастающим ускорением
    int32_t f = f_min;
    int32_t df = 0;
    int32_t ddf = ddf_max;

    result.motion_phases[PHASE_J1].f0 = f;
    result.motion_phases[PHASE_J1].df0 = df;
    result.motion_phases[PHASE_J1].ddf0 = ddf;

    debug_serial_printf("Phase J1:\r\n");
    while (df + ddf <= df_max) {
        if (Nj * 4 >= total_updates) { break; }

        f  += df;
        df += ddf;
        ++Nj;
        debug_serial_printf("[Step %d]: f = %d, df = %d, ddf = %d\r\n", Nj, f, df, ddf);
    }
    if (df > df_max) { df = df_max; }

    result.motion_phases[PHASE_J1].update_steps = Nj;

    const int64_t delta_J2 = Nj * df_max - ddf_max * Nj * (Nj - 1) / 2;

    // 2. Расчет фазы A1: Движение с постоянным ускорением
    // f = f
    // df = df
    ddf = 0;

    result.motion_phases[PHASE_A1].f0 = f;
    result.motion_phases[PHASE_A1].df0 = df;
    result.motion_phases[PHASE_A1].ddf0 = ddf;

    debug_serial_printf("Phase A1:\r\n");
    while (f + df <= f_max - delta_J2) {
        if (Nj * 4 + Na * 2 >= total_updates) { break; }

        f += df;
        ++Na;
        debug_serial_printf("[Step %d]: f = %d, df = %d, ddf = %d\r\n", Na, f, df, ddf);
    }

    result.motion_phases[PHASE_A1].update_steps = Na;

    // 3. Расчет фазы J2: Движение с убывающим ускорением
    // f = f
    // df = df
    ddf = -ddf_max;

    result.motion_phases[PHASE_J2].f0 = f;
    result.motion_phases[PHASE_J2].df0 = df;
    result.motion_phases[PHASE_J2].ddf0 = ddf;
    result.motion_phases[PHASE_J2].update_steps = Nj;

    debug_serial_printf("Phase J2:\r\n");
    for (uint32_t i = 0; i < Nj; ++i) {
        f  += df;
        df += ddf;
        debug_serial_printf("[Step %d]: f = %d, df = %d, ddf = %d\r\n", i, f, df, ddf);
    }

    DEBUG_ASSERT(f <= f_max); // Проверка соблюдения ограничений
    DEBUG_ASSERT(df == 0); // Проверка схождения расчета

    // 4. Расчет фазы C: Движение с постоянной скоростью
    // f = f
    // df = df
    ddf = 0;

    debug_serial_printf("Phase C:\r\n");
    if (total_updates > Nj * 4 + Na * 2) { Nc = total_updates - (Nj * 4 + Na * 2); }
    debug_serial_printf("[Steps %d]: f = %d, df = %d, ddf = %d\r\n", Nc, f, df, ddf);

    result.motion_phases[PHASE_C].f0 = f;
    result.motion_phases[PHASE_C].df0 = df;
    result.motion_phases[PHASE_C].ddf0 = ddf;
    result.motion_phases[PHASE_C].update_steps = Nc;

    DEBUG_ASSERT(f <= f_max);

    // 5. Расчет фазы J3: Движение с нарастающим торможением
    // f = f
    // df = df
    ddf = -ddf_max;

    // Обновление данных фазы J3
    result.motion_phases[PHASE_J3].f0 = f;
    result.motion_phases[PHASE_J3].df0 = df;
    result.motion_phases[PHASE_J3].ddf0 = ddf;
    result.motion_phases[PHASE_J3].update_steps = Nj;

    debug_serial_printf("Phase J3:\r\n");
    for (uint32_t i = 0; i < Nj; ++i) {
        f += df;
        df += ddf;
        debug_serial_printf("[Step %d]: f = %d, df = %d, ddf = %d\r\n", i, f, df, ddf);
    }

    // 6. Расчет фазы A2: Движение с постоянным торможением
    // f = f
    // df = df
    ddf = 0;

    // Обновление данных фазы A2
    result.motion_phases[PHASE_A2].f0 = f;
    result.motion_phases[PHASE_A2].df0 = df;
    result.motion_phases[PHASE_A2].ddf0 = ddf;
    result.motion_phases[PHASE_A2].update_steps = Na;

    debug_serial_printf("Phase A2:\r\n");
    for (uint32_t i = 0; i < Na; ++i) {
        f += df;
        debug_serial_printf("[Step %d]: f = %d, df = %d, ddf = %d\r\n", i, f, df, ddf);
    }

    // 7. Расчет фазы J4: Движение с убывающим торможением
    // f = f
    // df = df
    ddf = ddf_max;

    // Обновление данных фазы J4
    result.motion_phases[PHASE_J4].f0 = f;
    result.motion_phases[PHASE_J4].df0 = df;
    result.motion_phases[PHASE_J4].ddf0 = ddf;
    result.motion_phases[PHASE_J4].update_steps = Nj;

    debug_serial_printf("Phase J4:\r\n");
    for (uint32_t i = 0; i < Nj; ++i) {
        f = f + df;
        df = df + ddf;
        debug_serial_printf("[Step %d]: f = %d, df = %d, ddf = %d\r\n", i, f, df, ddf);
    }

    // Проверки сходимости расчета
    DEBUG_ASSERT(f == f_min);
    DEBUG_ASSERT(df == 0);
    DEBUG_ASSERT(Nj * 4 + Na * 2 + Nc == total_updates);

    return result;
}

