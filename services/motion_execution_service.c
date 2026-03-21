#include <services/motion_execution_service.h>

#include <globals/motor_telemetry_globals.h>

#include <core/service_timer.h>
#include <core/event_dispatcher.h>
#include <core/event_bus.h>
#include <core/event.h>

#include <services/debug_serial_service.h>
#include <services/motion_planning_service.h>

#include <drivers/step_timer_driver.h>
#include <drivers/motion_control_driver.h>
#include <drivers/time_driver.h>
#include <drivers/SysTick_driver.h>

#include <common/utils.h>
#include <common/debug_assert.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

static motion_executor_config_t config = {0};

const uint32_t CONST_ENA_DELAY_TIME_MS = 10;
const uint32_t CONST_DIR_DELAY_TIME_MS = 1;

// ======= Вспомогательные функции =======

static inline uint32_t get_period(uint32_t freq_hz) { return f_tim / freq_hz; } // Расчет периода для заданной частоты в тактах таймера

static inline void load_next_phase(void) { // Загрузка следующей фазы
    if (config.current_phase == PHASE_TAIL) { // Завершающая фаза
        step_timer_stop();
        config.is_running = false;
        g_generate_motor_telemetry_updates = false;
        motion_executor_notify(true);
        return;
    }
    
    do {
        ++config.current_phase; // Переход к следующей фазе

        if (config.current_phase == PHASE_TAIL && !config.current_block.tail_phase) { // Нет завершающей фазы
            step_timer_stop();
            config.is_running = false;
            g_generate_motor_telemetry_updates = false;
            motion_executor_notify(true);
            return;
        }
    } while (config.current_block.motion_phases[config.current_phase].update_steps == 0 && config.current_phase < PHASE_TAIL);
    
    const motion_phase_t* phase_handler = &config.current_block.motion_phases[config.current_phase]; // Загружаемая фаза
    
    // Копирование стартовых параметров
    config.f = phase_handler->f0;
    config.df = phase_handler->df0;
    config.ddf = phase_handler->ddf0;

    if (config.current_phase == PHASE_TAIL) {
        config.phase_steps_left = 1;
        step_timer_set_rcr(phase_handler->update_steps - 1);
        step_timer_update_timer();
    }
    else {
        config.phase_steps_left = phase_handler->update_steps;
    }
}

// ===== Публичные функции =====

void motion_executor_init(board_pin_e dir_pin, board_pin_e ena_pin) {
    DEBUG_ASSERT(!config.is_initialized);

    const step_timer_config_t pwm_config = {
        .prescaler = pwm_prescaler,
        .arr = f_tim / f_min,
        .ccr = f_tim / (f_min * 2),
        .rcr = repetitions - 1
    };

    step_timer_init(&pwm_config);
    motion_control_init(dir_pin, ena_pin);
    
    config.is_initialized = true;
}

void motion_executor_start(motion_block_t* block) { // Запуск выполнения
    DEBUG_ASSERT(block);

    // Остановка текущего движения
    step_timer_stop();
    step_timer_disable_irq();

    config.current_block = *block; // Сохранение блока

    config.current_phase = PHASE_J1; // Установка первой фазы
    motion_phase_t* phase_handler = &config.current_block.motion_phases[config.current_phase]; // Загружаемая фаза

    debug_serial_printf("dir = %d, tail = %d\r\n", config.current_block.reverse_direction, config.current_block.tail_phase);
    for (uint8_t i = 0; i < PHASE_COUNT; ++i) {
        debug_serial_printf("upd = %d, f0 = %d, df0 = %d, ddf0 = %d\r\n", config.current_block.motion_phases[i].update_steps, config.current_block.motion_phases[i].f0, config.current_block.motion_phases[i].df0, config.current_block.motion_phases[i].ddf0);
    }
    
    while (phase_handler->update_steps == 0) {
        if (config.current_phase > PHASE_TAIL) { return; }
        ++config.current_phase;
        phase_handler = &config.current_block.motion_phases[config.current_phase]; // Загружаемая фаза
    }

    debug_serial_printf("loaded_phase = %d\r\n", config.current_phase);
    
    // Копирование стартовых параметров
    config.f = phase_handler->f0;
    config.df = phase_handler->df0;
    config.ddf = phase_handler->ddf0;
    config.phase_steps_left = phase_handler->update_steps;

    step_timer_set_period(get_period(config.f)); // Настройка частоты
    step_timer_enable_irq(); // Включение прерываний

    config.is_running = true;

    g_generate_motor_telemetry_updates = true;

    set_motion_control_enable(true); // Включение управления
    delay_ms(CONST_ENA_DELAY_TIME_MS);

    set_motion_control_direction(config.current_block.reverse_direction); // Установка направления
    delay_ms(CONST_DIR_DELAY_TIME_MS);

    step_timer_start(); // Запуск таймера
    service_timer_enable(); // Запуск служебного таймера
}

void motion_executor_stop(void) { // Программная остановка выполнения
    step_timer_disable_irq(); // Отключение прерываний таймера
    step_timer_disable_output(); // Отключение выхода сигнала
    step_timer_stop(); // Остановка таймера

    set_motion_control_enable(false); // Отключение двигателя

    config.is_running = false;
    g_generate_motor_telemetry_updates = false;
}

//bool motion_executor_is_running(void) { return config.is_running; }

void motion_executor_notify(bool state) {
        event_bus_t *bus = event_dispatcher_get_bus();
        event_t evt = {0};

        if (state) {
            evt.id = EVENT_MOTOR_ROTATION_COMPLETE;
        }
        else { // Формирование уведомления об аварийной остановке двигателя
            evt.id = EVENT_MOTOR_EMERGENCY_STOP;
        }

        evt.priority = EVENT_PRIORITY_HIGH;
        evt.flags = EVENT_FLAG_NONE;
        evt.timestamp = g_SysTick_cnt;

        event_bus_post_from_isr(bus, &evt); // Публикация события
}

void motion_executor_telemetry_update(void) {
    uint32_t phase_updates_milestone = 0;

    for (uint8_t i = 0; i <= config.current_phase; ++i) {
        phase_updates_milestone += ((i == PHASE_TAIL) ? 1 : config.current_block.motion_phases[i].update_steps);
    }

    g_motor_telemetry.active_phase = config.current_phase;

    if (config.current_phase == PHASE_TAIL) {
        g_motor_telemetry.progress_percentage = 100;
    }
    else {
        //uint32_t percentage = ((phase_updates_milestone - config.phase_steps_left) * 1000) / config.current_block.total_updates;
        //g_motor_telemetry.progress_percentage = (percentage + 5) / 10;
        uint32_t div = config.current_block.total_updates;
        uint32_t mul1 = phase_updates_milestone - config.phase_steps_left;
        uint32_t temp_gcd = gcd(mul1, div);
        mul1 /= temp_gcd;
        div /= temp_gcd;

        uint32_t mul2 = 1000;
        temp_gcd = gcd(mul2, div);
        mul2 /= temp_gcd;
        div /= temp_gcd;

        //debug_serial_printf("mul1 = %u\r\n", mul1);
        //debug_serial_printf("mul2 = %u\r\n", mul2);
        //debug_serial_printf("div = %u\r\n", div);

        DEBUG_ASSERT((uint64_t)mul1 * (uint64_t)mul2 < UINT32_MAX);

        g_motor_telemetry.progress_percentage = mul1 * mul2 / div;
    }
}

// ======= Обработчики прерываний =======

void TIM1_UP_Handler(void) { // Прерывание обновления фазы
    // 0. Обновление флага прерывания
    step_timer_reset_update_flag();

    // 1. Обновление параметров
    config.f += config.df;
    config.df += config.ddf;

    // 2. Настройка периода таймера
    step_timer_set_period(get_period(config.f));

    // 3. Декремент оставшихся обновлений
    --config.phase_steps_left;

    // 4. Загрузка следующей фазы, если необходимо
    if (config.phase_steps_left == 0) { load_next_phase(); }
}

void TIM1_BRK_Handler(void) { // Прерывание аварийной остановки
    // 0. Обновление флага прерывания
    step_timer_reset_breakup_flag();

    if (config.is_running) {
        motion_executor_stop();
        motion_executor_notify(false);
    }
}