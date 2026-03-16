#include <services/debug_serial_service.h>
#include <globals/USART_globals.h>
#include <core/event_dispatcher.h>
#include <services/command_parser.h>
//#include <drivers/time_driver.h>
#include <common/debug_assert.h>

// Статические буферы
#define DEBUG_TX_BUFFER_SIZE 128
#define DEBUG_RX_BUFFER_SIZE 64
#define SERIAL_CMD_BUFFER_SIZE 64   // можно потом вынести в конфиг

static char serial_cmd_buffer[SERIAL_CMD_BUFFER_SIZE];
static uint16_t serial_cmd_len = 0;
static const char *hex = "0123456789ABCDEF";

static USART_driver_t serial_USART_driver;
static uint8_t TX_buffer[DEBUG_TX_BUFFER_SIZE];
static uint8_t RX_buffer[DEBUG_RX_BUFFER_SIZE];
static bool initialized = false;

bool debug_serial_init(USART_port_e port, uint32_t baudrate, bool remap, bool enable_stats) {
    DEBUG_ASSERT(!initialized);
    
    USART_config_t cfg = {0};
    cfg.port = port;
    cfg.baudrate = baudrate;
    cfg.remap = remap;
    cfg.enable_stats = enable_stats;
    
    initialized = USART_init(&serial_USART_driver, &cfg, TX_buffer, DEBUG_TX_BUFFER_SIZE, RX_buffer, DEBUG_RX_BUFFER_SIZE);
    if (!initialized) { return false; }

    event_bus_t *bus = event_dispatcher_get_bus();
    DEBUG_ASSERT(bus);

    event_bus_subscribe(bus, EVENT_USART1_RX, debug_serial_handle_event);
    event_bus_subscribe(bus, EVENT_USART2_RX, debug_serial_handle_event);
    event_bus_subscribe(bus, EVENT_USART3_RX, debug_serial_handle_event);
    serial_cmd_len = 0;

    return true;
}

void debug_serial_putchar(uint8_t ch) {
    DEBUG_ASSERT(initialized);
    USART_try_send(&serial_USART_driver, ch, 10);
    //USART_send(&serial_USART_driver, (uint8_t)ch);
}

void debug_serial_puts(const char *str) {
    DEBUG_ASSERT(initialized);
    DEBUG_ASSERT(str);

    const char *ptr = str;
    while (*ptr) { char ch = *ptr++; debug_serial_putchar(ch); } // Поиск конца строки
}

void debug_serial_putu(uint32_t num) {
    DEBUG_ASSERT(initialized);
    
    char buf[11];
    uint8_t i = 0;
    
    if (num == 0) {
        debug_serial_putchar('0');
        return;
    }
    
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        debug_serial_putchar(buf[--i]);
    }
}

void debug_serial_puti(int32_t num) { // Вывод знакового числа
    DEBUG_ASSERT(initialized);
    
    if (num < 0) {
        debug_serial_putchar('-');
        debug_serial_putu((uint32_t)(-num));
    } else {
        debug_serial_putu((uint32_t)num);
    }
}

void debug_serial_printf(const char *format, ...) { // Форматированный вывод
    DEBUG_ASSERT(initialized);
    DEBUG_ASSERT(format);
    
    va_list args;
    va_start(args, format);
    
    const char *p = format;
    while (*p) {
        if (*p == '%') {
            ++p;
            switch (*p) {
                case 's': { // строка
                    const char *str = va_arg(args, const char*);
                    debug_serial_puts(str ? str : "(null)");
                    break;
                }
                case 'c': { // символ
                    int c = va_arg(args, int);
                    debug_serial_putchar((char)c);
                    break;
                }
                case 'd':
                case 'i': { // знаковое число
                    int32_t val = va_arg(args, int32_t);
                    debug_serial_puti(val);
                    break;
                }
                case 'u': { // беззнаковое число
                    uint32_t val = va_arg(args, uint32_t);
                    debug_serial_putu(val);
                    break;
                }
                case 'x':
                case 'X': { // шестнадцатеричное
                    uint32_t val = va_arg(args, uint32_t);
                    debug_serial_puts("0x");
                    for (int8_t i = 28; i >= 0; i -= 4) {
                        debug_serial_putchar(hex[(val >> i) & 0xF]);
                    }
                    break;
                }
                case 'p': { // указатель
                    void *ptr = va_arg(args, void*);
                    if (ptr) {
                        uint32_t val = (uint32_t)ptr;
                        debug_serial_puts("0x");
                        for (int8_t i = 28; i >= 0; i -= 4) {
                            debug_serial_putchar(hex[(val >> i) & 0xF]);
                        }
                    } else {
                        debug_serial_puts("(NULL)"); // Стандартное представление NULL
                    }
                    break;
                }
                case '%': { // символ процента
                    debug_serial_putchar('%');
                    break;
                }
                default: { // неизвестный спецификатор
                    debug_serial_putchar('%');
                    debug_serial_putchar(*p);
                    break;
                }
            }
        } else {
            debug_serial_putchar(*p);
        }
        ++p;
    }
    
    va_end(args);
}

bool debug_serial_getchar(uint8_t *ch) { // Чтение символа
    DEBUG_ASSERT(initialized && ch);
    return USART_receive(&serial_USART_driver, ch);
}

void serial_print_stats(void) {
    debug_serial_printf("Serial stats:\r\n");
    debug_serial_printf("B R/S: %u %u, E F/N/P/O: %u %u %u %u, O T/R: %u %u, I T/R: %u %u, ISR: %u\r\n",
        serial_USART_driver.stats.bytes_received,
        serial_USART_driver.stats.bytes_sent,
        serial_USART_driver.stats.framing_errors,
        serial_USART_driver.stats.noise_errors,
        serial_USART_driver.stats.parity_errors,
        serial_USART_driver.stats.overrun_errors,
        serial_USART_driver.stats.tx_buffer_overflows,
        serial_USART_driver.stats.rx_buffer_overflows,
        serial_USART_driver.stats.tx_idle,
        serial_USART_driver.stats.rx_idle,
        serial_USART_driver.stats.isr_calls
    );
}

void debug_serial_handle_event(const event_t *evt) {
    if (evt->id < EVENT_USART1_RX || evt->id > EVENT_USART3_RX) {
        return;
    }

    USART_port_e port = (USART_port_e)(evt->id - EVENT_USART1_RX);
    uint8_t byte;

    // Читаем все доступные байты из RX буфера
    while (USART_receive(&serial_USART_driver, &byte)) {

        // Игнорируем '\r'
        if (byte == '\r') {
            continue;
        }

        // Если конец строки
        if (byte == '\n') {

            // Завершаем строку
            if (serial_cmd_len < SERIAL_CMD_BUFFER_SIZE) {
                serial_cmd_buffer[serial_cmd_len] = '\0';
            }

            // Вызываем парсер
            bool ok = command_parser_parse_and_post(
                event_dispatcher_get_bus(),
                serial_cmd_buffer
            );

            if (!ok) {
                debug_serial_puts("Error: invalid command\r\n");
            }

            // Сбрасываем буфер для следующей команды
            serial_cmd_len = 0;

            // Сбрасываем флаг ожидания для этого USART
            g_usart_rx_pending[port] = false;
            return;
        }

        // Обычный символ — добавляем в буфер
        if (serial_cmd_len < SERIAL_CMD_BUFFER_SIZE - 1) {
            serial_cmd_buffer[serial_cmd_len++] = (char)byte;
        }
        else {
            // ПЕРЕПОЛНЕНИЕ БУФЕРА КОМАНДЫ
            serial_cmd_len = 0;

            // Очищаем RX буфер полностью
            uint8_t tmp;
            while (USART_receive(&serial_USART_driver, &tmp)) {}

            debug_serial_puts("Error: command too long\r\n");

            // Сбрасываем флаг и выходим
            g_usart_rx_pending[port] = false;
            return;
        }
    }
}
