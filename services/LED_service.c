#include <services/LED_service.h>

#include <globals/LED_globals.h>

#include <core/service_timer.h>

#include <drivers/time_driver.h>

#include <common/debug_assert.h>

#include <services/debug_serial_service.h>

static LED_service_t svc = {0};

bool LED_service_init_led(uint8_t led_id, board_pin_e pin_code, bool inverted) {
    DEBUG_ASSERT(led_id < CONST_LED_SERVICE_MAX_LEDS);

    if (!LED_init(pin_code, inverted, &svc.led[led_id])) { // Инициализация аппаратного LED
        svc.led[led_id].port = 0;
        svc.led[led_id].pin = 0;
        return false;
    }

    // Инициализация конфигурации командного паттерна
    svc.config[led_id].exec_pattern = NULL;
    svc.config[led_id].last_change_ms = 0;
    svc.config[led_id].curr_indx = 0;
    svc.config[led_id].repeat = false;

    if (led_id + 1 > svc.led_num) { svc.led_num = led_id + 1; } // Фиксация размера массива
    return true;
}

void LED_service_execute(uint8_t led_id, const LED_pattern_t* pattern, bool repeat) {
    DEBUG_ASSERT(led_id < CONST_LED_SERVICE_MAX_LEDS);
    DEBUG_ASSERT(pattern && pattern->arr_cmd_seq && pattern->cmd_seq_length);

    LED_set(&svc.led[led_id], false); // Выключение диода

    // Подключение командного паттерна
    svc.config[led_id].exec_pattern = pattern;
    svc.config[led_id].repeat = repeat;
    svc.config[led_id].curr_indx = 0;
    svc.config[led_id].last_change_ms = get_current_time_ms();

    const LED_command_t* cmd = &pattern->arr_cmd_seq[0];
    LED_set(&svc.led[led_id], cmd->action);

    g_generate_led_updates = true; // Включение генерации обновления состояний
    service_timer_enable();
    // Выполнение паттерна произойдет при следующем вызове LED_service_update
}

void LED_service_set_led_state(uint8_t led_id, bool state) {
    DEBUG_ASSERT(led_id < CONST_LED_SERVICE_MAX_LEDS);

    svc.config[led_id].exec_pattern = NULL; // Остановка активного паттерна
    LED_set(&svc.led[led_id], state); // Установка состояния диода
    // Выключение генерации обновления состояний произойдет при следующем вызове LED_service_update
}

void LED_service_update()
{
    bool any_active = false;

    for (uint8_t i = 0; i < svc.led_num; ++i) {
        if (svc.config[i].exec_pattern == NULL) { continue; } // Проверка наличия активного паттерна

        any_active = true;
        const LED_command_t* cmd = &(svc.config[i].exec_pattern->arr_cmd_seq[svc.config[i].curr_indx]); // Активная команда
        const uint32_t current_time = get_current_time_ms();
        const uint32_t elapsed_time = get_elapsed_time_ms(svc.config[i].last_change_ms, current_time);

        if (elapsed_time >= cmd->duration_ms) { // Необходимый интервал прошел
            ++(svc.config[i].curr_indx); // Переход к следующей команде

            if (svc.config[i].curr_indx >= svc.config[i].exec_pattern->cmd_seq_length) {
                if (svc.config[i].repeat) {
                    svc.config[i].curr_indx = 0; // Повтор включен
                } else {
                    svc.config[i].exec_pattern = NULL; // Паттерн завершён
                    continue;
                }
            }

            const LED_command_t* new_cmd = &(svc.config[i].exec_pattern->arr_cmd_seq[svc.config[i].curr_indx]); // Новая активная команда

            LED_set(&svc.led[i], new_cmd->action); // Выполнить
            svc.config[i].last_change_ms = get_current_time_ms(); // Запись метки времени
        }
    }
    g_generate_led_updates = any_active; // Запись флага
}

void LED_service_handle_event(const event_t *evt) {
    if (evt->id != EVENT_LED_CONTROL) {
        return;
    }

    uint32_t val = evt->payload.data.unsigned_value;
    uint8_t led_id = (uint8_t)(val >> 1);
    bool state = (val & 1U) ? true : false;

    if (led_id < CONST_LED_SERVICE_MAX_LEDS && led_id < svc.led_num) {
        LED_service_set_led_state(led_id, state);
        debug_serial_printf("[%u] LED CHANGE\n", g_SysTick_cnt);
    }
    else {
        debug_serial_puts("Error: invalid LED id\r\n");
    }
}