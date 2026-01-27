#include <services/serial_debug_service.h>
#include <common/macro_debug.h>

// Статические буферы
#define DEBUG_TX_BUFFER_SIZE 128
#define DEBUG_RX_BUFFER_SIZE 64

static const uint32_t serial_flush_timeout_tick = 1000;
static const char *hex = "0123456789ABCDEF";
static volatile uint32_t tx_drop_counter = 0;

static s_USART_driver serial_USART_driver;
static uint8_t TX_buffer[DEBUG_TX_BUFFER_SIZE];
static uint8_t RX_buffer[DEBUG_RX_BUFFER_SIZE];
static bool initialized = false;

bool serial_debug_init(e_USART_port port, uint32_t baudrate, bool remap_pins, bool need_stats) {
    DEBUG_STATIC_CHECK_FALSE_RET(!initialized, false);
    
    s_USART_config cfg = {0};
    cfg.port = port;
    cfg.baudrate = baudrate;
    cfg.remap_pins = remap_pins;
    
    initialized = USART_init(&serial_USART_driver, &cfg, TX_buffer, DEBUG_TX_BUFFER_SIZE, RX_buffer, DEBUG_RX_BUFFER_SIZE, need_stats);
    return initialized;
}

void serial_debug_putchar(char ch) {
    DEBUG_STATIC_CHECK_FALSE(initialized);
    if (!USART_send(&serial_USART_driver, (uint8_t)ch)) {
        ++tx_drop_counter;
    }
}

static inline void serial_print_drop_stats(void) {
    serial_debug_printf("TX bytes dropped: %u\r\n", tx_drop_counter);
}

void serial_debug_puts(const char *str) {
    DEBUG_STATIC_CHECK_FALSE(initialized);
    DEBUG_STATIC_CHECK_FALSE(str);

    const char *end = str;
    while (*end) { end++; } // Поиск конца строки
    USART_send_sequence(&serial_USART_driver, (const uint8_t*)str, (uint16_t)(end - str));
}

void serial_debug_putu(uint32_t num) {
    DEBUG_STATIC_CHECK_FALSE(initialized);
    
    char buf[11];
    uint8_t i = 0;
    
    if (num == 0) {
        serial_debug_putchar('0');
        return;
    }
    
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        serial_debug_putchar(buf[--i]);
    }
}

void serial_debug_putu_binary(uint32_t num) {  // Вывод беззнакового числа
    DEBUG_STATIC_CHECK_FALSE(initialized);

    uint8_t bytes[4] = { // Отправка как 4 байта
        (uint8_t)(num >> 24),
        (uint8_t)(num >> 16),
        (uint8_t)(num >> 8),
        (uint8_t)num
    };
    USART_send_sequence(&serial_USART_driver, bytes, 4);
}

void serial_debug_puti(int32_t num) { // Вывод знакового числа
    DEBUG_STATIC_CHECK_FALSE(initialized);
    
    if (num < 0) {
        serial_debug_putchar('-');
        serial_debug_putu((uint32_t)(-num));
    } else {
        serial_debug_putu((uint32_t)num);
    }
}

void serial_debug_printf(const char *format, ...) { // Форматированный вывод
    DEBUG_STATIC_CHECK_FALSE(initialized);
    DEBUG_STATIC_CHECK_FALSE(format);
    
    va_list args;
    va_start(args, format);
    
    const char *p = format;
    while (*p) {
        if (*p == '%') {
            ++p;
            switch (*p) {
                case 's': { // строка
                    const char *str = va_arg(args, const char*);
                    serial_debug_puts(str ? str : "(null)");
                    break;
                }
                case 'c': { // символ
                    int c = va_arg(args, int);
                    serial_debug_putchar((char)c);
                    break;
                }
                case 'd':
                case 'i': { // знаковое число
                    int32_t val = va_arg(args, int32_t);
                    serial_debug_puti(val);
                    break;
                }
                case 'u': { // беззнаковое число
                    uint32_t val = va_arg(args, uint32_t);
                    serial_debug_putu(val);
                    break;
                }
                case 'x':
                case 'X': { // шестнадцатеричное
                    uint32_t val = va_arg(args, uint32_t);
                    serial_debug_puts("0x");
                    for (int8_t i = 28; i >= 0; i -= 4) {
                        serial_debug_putchar(hex[(val >> i) & 0xF]);
                    }
                    break;
                }
                case 'p': { // указатель
                    void *ptr = va_arg(args, void*);
                    if (ptr) {
                        uint32_t val = (uint32_t)ptr;
                        serial_debug_puts("0x");
                        for (int8_t i = 28; i >= 0; i -= 4) {
                            serial_debug_putchar(hex[(val >> i) & 0xF]);
                        }
                    } else {
                        serial_debug_puts("(NULL)"); // Стандартное представление NULL
                    }
                    break;
                }
                case '%': { // символ процента
                    serial_debug_putchar('%');
                    break;
                }
                default: { // неизвестный спецификатор
                    serial_debug_putchar('%');
                    serial_debug_putchar(*p);
                    break;
                }
            }
        } else {
            serial_debug_putchar(*p);
        }
        ++p;
    }
    
    va_end(args);
}

bool serial_debug_getchar(uint8_t *ch) { // Чтение символа
    DEBUG_STATIC_CHECK_FALSE_RET(initialized, false);
    DEBUG_STATIC_CHECK_FALSE_RET(ch, false);
    return USART_receive(&serial_USART_driver, ch);
}

uint16_t serial_debug_available(void) { // Количество доступных символов
    DEBUG_STATIC_CHECK_FALSE_RET(initialized, 0);
    return USART_RX_available(&serial_USART_driver);
}

void serial_debug_flush(void) { // Ожидание отправки всех данных
    DEBUG_STATIC_CHECK_FALSE(initialized);
    USART_flush(&serial_USART_driver, serial_flush_timeout_tick);
}

bool serial_debug_is_busy(void) { // Проверка доступности
    DEBUG_STATIC_CHECK_FALSE_RET(initialized, false);
    return USART_TX_is_busy(&serial_USART_driver);
}

void serial_debug_echo_simple(void) {
    DEBUG_STATIC_CHECK_FALSE(initialized);
    
    static uint8_t ch;
    if (serial_debug_getchar(&ch)) { // Проверка получения символа
        serial_debug_putchar(ch);
        if (ch == '\r') {
            serial_debug_putchar('\n');
        }
    }
}

void serial_print_stats(void) {
    serial_debug_printf("Serial debug service stats:\r\n");
    serial_debug_printf("BR: %u, BS: %u, FE: %u, NE: %u, PE: %u, OE: %u\r\n",
        serial_USART_driver.stats.bytes_received,
        serial_USART_driver.stats.bytes_sent,
        serial_USART_driver.stats.framing_errors,
        serial_USART_driver.stats.noise_errors,
        serial_USART_driver.stats.parity_errors,
        serial_USART_driver.stats.overrun_errors
    );
    if (tx_drop_counter) { serial_print_drop_stats(); tx_drop_counter = 0; }
}