#include <drivers/SysTick_driver.h>
#include <services/command_parser.h>
#include <core/event.h>
#include <services/LED_service.h>
#include <common/debug_assert.h>

static inline bool is_digit(char c) { return (c >= '0' && c <= '9'); }

static inline bool str_equal(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return false;
        ++a; ++b;
    }
    return (*a == '\0' && *b == '\0');
}

bool command_parser_parse_and_post(event_bus_t *bus, const char *cmd) {
    DEBUG_ASSERT(bus && cmd);
    event_t evt;

    uint16_t cmd_len = 0;
    while (cmd[cmd_len] != '\0') {
        ++cmd_len;
    }

    // Ищем начало команды "led"
    if (cmd_len > 4 && cmd[0] == 'l' && cmd[1] == 'e' && cmd[2] == 'd' && cmd[3] == ' ') {

        // Пропускаем "led "
        const char *p = cmd + 4;

        // Читаем номер LED
        if (!is_digit(*p)) {
            return false;
        }

        uint8_t led_id = (uint8_t)(*p - '0');
        ++p;

        // Должен быть пробел
        if (*p != ' ') {
            return false;
        }
        ++p;

        // Определяем действие
        bool state;

        if (str_equal(p, "on")) {
            state = true;
        }
        else if (str_equal(p, "off")) {
            state = false;
        }
        else {
            return false;
        }

        // Формируем событие
        evt.id = EVENT_LED_CONTROL;
        evt.priority = EVENT_PRIORITY_NORMAL;
        evt.flags = EVENT_FLAG_NONE;
        evt.timestamp = g_SysTick_cnt;

        evt.payload.type = EVENT_DATA_UNSIGNED;
        evt.payload.data.unsigned_value = ((uint32_t)led_id << 1) | (state ? 1U : 0U);
    }
    else if (cmd_len > 5 && cmd[0] == 't' && cmd[1] == 'e' && cmd[2] == 's' && cmd[3] == 't' && cmd[4] == ' ') {
        const char *p = cmd + 5;

        int32_t f = 0;
        uint8_t i = 0;

        do {
            if (!is_digit(*p)) { break; }
            f = f * 10 + (uint32_t)(*p - '0');
            ++p;
            ++i;
        } while(i < 7);

        if (f == 0) { return false; }

        // Формируем событие
        evt.id = EVENT_MOTOR_ROTATION_TEST_REQUEST;
        evt.priority = EVENT_PRIORITY_NORMAL;
        evt.flags = EVENT_FLAG_NONE;
        evt.timestamp = g_SysTick_cnt;

        evt.payload.type = EVENT_DATA_SIGNED;
        evt.payload.data.signed_value = f;
    }
    else if (cmd_len > 7 && cmd[0] == 'r' && cmd[1] == 'o' && cmd[2] == 't' && cmd[3] == 'a' && cmd[4] == 't' && cmd[5] == 'e' && cmd[6] == ' ') {
        const char *p = cmd + 7;

        int32_t angle = 0;
        uint8_t i = 0;
        bool direction = true;

        if (*p == '-') { direction = false; ++p; }

        do {
            if (!is_digit(*p)) { break; }
            angle = angle * 10 + (uint32_t)(*p - '0');
            ++p;
            ++i;
        } while(i < 10);

        if (angle == 0) { return false; }

        angle = direction ? angle : (-1 * angle);

        // Формируем событие
        evt.id = EVENT_MOTOR_ROTATION_REQUEST;
        evt.priority = EVENT_PRIORITY_NORMAL;
        evt.flags = EVENT_FLAG_NONE;
        evt.timestamp = g_SysTick_cnt;

        evt.payload.type = EVENT_DATA_SIGNED;
        evt.payload.data.signed_value = angle;
    }
    /*else if (cmd[0] == 'p' && cmd[1] == 'r' && cmd[2] == 's' && cmd[3] == 'c' && cmd[4] == ' ') {
        const char *p = cmd + 5;

        uint16_t prescaler = 0;
        uint8_t i = 0;
        do {
            if (!is_digit(*p)) { break; }
            prescaler = prescaler * 10 + (uint16_t)(*p - '0');
            ++p;
            ++i;
        } while(i < 16);

        if (prescaler == 0) { return false; }

        // Формируем событие
        evt.id = EVENT_PWM_CHANGE_PRESCALER;
        evt.priority = EVENT_PRIORITY_NORMAL;
        evt.flags = EVENT_FLAG_NONE;
        evt.timestamp = g_SysTick_cnt;

        evt.payload.type = EVENT_DATA_UNSIGNED;
        evt.payload.data.unsigned_value = (uint32_t)prescaler;
    }*/
    else { return false; }
    return event_bus_post(bus, &evt);
}
