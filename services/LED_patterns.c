#include <services/LED_patterns.h>

#define MACRO_LED_COMMAND(action, ms)  { (action),  (ms) } // Макрос создания команды

// 1. Blink 500: ВКЛ/ВЫКЛ по 500 мс
static const s_LED_command blink_500_seq[] = {
    MACRO_LED_COMMAND(true,  500),
    MACRO_LED_COMMAND(false, 500)
};
const s_LED_service_config LED_pattern_blink_500 = {
    .arr_cmd_seq = blink_500_seq,
    .cmd_seq_length = sizeof(blink_500_seq) / sizeof(blink_500_seq[0]),
};

// 2. Blink 1000: ВКЛ/ВЫКЛ по 1 с
static const s_LED_command blink_1000_seq[] = {
    MACRO_LED_COMMAND(true,  1000),
    MACRO_LED_COMMAND(false, 1000)
};
const s_LED_service_config LED_pattern_blink_1000 = {
    .arr_cmd_seq = blink_1000_seq,
    .cmd_seq_length = sizeof(blink_1000_seq) / sizeof(blink_1000_seq[0]),
};

// 3. Heartbeat: имитация пульса (короткое мигание + пауза)
static const s_LED_command heartbeat_seq[] = {
    MACRO_LED_COMMAND(true,   100),  // Короткий импульс
    MACRO_LED_COMMAND(false, 400),  // Пауза
    MACRO_LED_COMMAND(true,   100),  // Второй импульс
    MACRO_LED_COMMAND(false, 900)   // Длинная пауза
};
const s_LED_service_config LED_pattern_heartbeat = {
    .arr_cmd_seq = heartbeat_seq,
    .cmd_seq_length = sizeof(heartbeat_seq) / sizeof(heartbeat_seq[0]),
};

// 4. SOS: Morse code (· · · – – – – · · ·)
// Точка = 200 мс, тире = 600 мс, паузы между элементами = 200 мс
static const s_LED_command sos_seq[] = {
    // S: · · ·
    MACRO_LED_COMMAND(true,  200), MACRO_LED_COMMAND(false, 200),
    MACRO_LED_COMMAND(true,  200), MACRO_LED_COMMAND(false, 200),
    MACRO_LED_COMMAND(true,  200), MACRO_LED_COMMAND(false, 600),  // пауза между буквами
    // O: – – –
    MACRO_LED_COMMAND(true,  600), MACRO_LED_COMMAND(false, 200),
    MACRO_LED_COMMAND(true,  600), MACRO_LED_COMMAND(false, 200),
    MACRO_LED_COMMAND(true,  600), MACRO_LED_COMMAND(false, 600),  // пауза между буквами
    // S: · · ·
    MACRO_LED_COMMAND(true,  200), MACRO_LED_COMMAND(false, 200),
    MACRO_LED_COMMAND(true,  200), MACRO_LED_COMMAND(false, 200),
    MACRO_LED_COMMAND(true,  200), MACRO_LED_COMMAND(false, 1000)  // длинная пауза в конце
};
const s_LED_service_config LED_pattern_SOS = {
    .arr_cmd_seq = sos_seq,
    .cmd_seq_length = sizeof(sos_seq) / sizeof(sos_seq[0]),
};