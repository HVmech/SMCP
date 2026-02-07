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

    // Ищем начало команды "led"
    if (cmd[0] != 'l' || cmd[1] != 'e' || cmd[2] != 'd' || cmd[3] != ' ') {
        return false;
    }

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
    event_t evt = {
        .id = EVENT_LED_CONTROL,
        .priority = EVENT_PRIORITY_NORMAL,
        .flags = EVENT_FLAG_NONE,
        .timestamp = 0
    };

    evt.payload.type = EVENT_DATA_UNSIGNED;
    evt.payload.data.unsigned_value = ((uint32_t)led_id << 1) | (state ? 1U : 0U);

    return event_bus_post(bus, &evt);
}
