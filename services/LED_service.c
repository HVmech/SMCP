
#include <services/LED_service.h>
#include <drivers/time_driver.h>
#include <common/debug_assert.h>

bool LED_service_init(s_LED_service* instance, const s_LED_service_config* config, e_board_pin pin_code, bool inverted, bool repeat) {
    DEBUG_ASSERT(instance && config && config->arr_cmd_seq && config->cmd_seq_length);

    // Заполнение полей
    instance->config = config;
    instance->curr_indx = 0;
    instance->start_time_ms = 0;
    instance->active = false;
    instance->repeat = repeat;

    if (!LED_init(pin_code, inverted, &instance->led)) { // Проверка инициализации дочерней структуры - диода
        // Заполнение неинициализированных полей
        instance->led.port = 0;
        instance->led.pin = 0;
        return false;
    }
    return true;
}

void LED_service_start(s_LED_service* instance) {
    DEBUG_ASSERT(instance);

    // Заполнение полей
    instance->curr_indx = 0;
    instance->active = true;
    instance->start_time_ms = get_current_time_ms();

    const s_LED_command* cmd = &instance->config->arr_cmd_seq[0]; // Первая команда
    LED_set(&instance->led, cmd->action);
}

void LED_service_stop(s_LED_service* instance) {
    DEBUG_ASSERT(instance);
    instance->active = false; // Выключение службы
    LED_set(&instance->led, LED_OFF);
}

inline bool LED_service_is_running(const s_LED_service* instance) {
    DEBUG_ASSERT(instance);
    return instance->active;
}

void LED_service_update(s_LED_service* instance) {
    DEBUG_ASSERT(instance);
    if (instance->active) { // Проверка активации службы
        const s_LED_service_config* config = instance->config;
        const s_LED_command* cmd = &config->arr_cmd_seq[instance->curr_indx];
        const bool is_infinite = (cmd->duration_ms == LED_DURATION_INFINITE);

        if (!is_infinite) {
            const uint32_t current_time = get_current_time_ms();
            const uint32_t elapsed_time = get_elapsed_time_ms(instance->start_time_ms);

            if (elapsed_time >= cmd->duration_ms) { // Если время выполнения команды превышено
                instance->curr_indx++; // Переходим к следующей команде

                if (instance->curr_indx >= instance->config->cmd_seq_length) { // Проверка достижения конца последовательности команд
                    if (!instance->repeat) { // Проверка признака циклического выполнения
                        instance->active = false; // Отключение службы
                        LED_set(&instance->led, LED_OFF);
                        return;
                    }
                    instance->curr_indx = 0; // Возврат к выполнению первой команды
                }

                // Обновление параметров команды
                instance->start_time_ms = current_time;
                cmd = &instance->config->arr_cmd_seq[instance->curr_indx];

                LED_set(&instance->led, cmd->action); // Выполнение команды
            }
        }
    }
}

void LED_service_execute(s_LED_service* instance, const s_LED_service_config* config, bool repeat) {
    DEBUG_ASSERT(instance && config && config->arr_cmd_seq && config->cmd_seq_length);

    if (LED_service_is_running(instance)) { LED_service_stop(instance); } // Остановка службы

    // Обновление конфигурации
    instance->config = config;
    instance->curr_indx = 0;
    instance->start_time_ms = 0;
    instance->repeat = repeat;

    LED_service_start(instance); // Выполнеие новой последовательности команд
}
