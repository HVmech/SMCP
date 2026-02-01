#include <services/debug_serial_service.h>
#include <common/debug_assert.h>

// Статические буферы
#define DEBUG_TX_BUFFER_SIZE 128
#define DEBUG_RX_BUFFER_SIZE 64

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
    return initialized;
}

void debug_serial_putchar(char ch) {
    DEBUG_ASSERT(initialized);
    USART_try_send(&serial_USART_driver, (uint8_t)ch, 10);
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

void debug_serial_echo_simple(void) {
    DEBUG_ASSERT(initialized);
    
    static uint8_t ch;
    if (debug_serial_getchar(&ch)) { // Проверка получения символа
        debug_serial_putchar(ch);
        if (ch == '\r') {
            debug_serial_putchar('\n');
        }
    }
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