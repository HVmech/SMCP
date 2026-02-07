#include <drivers/USART_driver.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/common.h>
#include <libopencm3/cm3/nvic.h>

#include <globals/USART_globals.h>

#include <core/service_timer.h>

#include <drivers/SysTick_driver.h>

#include <common/debug_assert.h>
#include <common/asm.h>

#include <services/debug_serial_service.h>
#include <drivers/time_driver.h>

static const uint32_t USART_port_map[USART_CNT] = {USART1, USART2, USART3};
static const uint8_t USART_IRQ_map[USART_CNT] = {NVIC_USART1_IRQ, NVIC_USART2_IRQ, NVIC_USART3_IRQ};

static USART_driver_t *USART_driver_list[USART_CNT] = {0};

static inline uint32_t USART_get_port_address(USART_port_e port) {
    DEBUG_ASSERT(port < USART_CNT);
    return USART_port_map[port];
}

static inline uint8_t USART_get_IRQ_number(USART_port_e port) {
    DEBUG_ASSERT(port < USART_CNT);
    return USART_IRQ_map[port];
}

static inline bool USART_has_errors(uint32_t sr) {
    return sr & (USART_SR_ORE | USART_SR_NE | USART_SR_FE | USART_SR_PE);
}

static inline bool USART_TX_is_ready(uint32_t sr) {
    return sr & USART_SR_TXE;
}

static inline bool USART_RX_is_ready(uint32_t sr) {
    return sr & USART_SR_RXNE;
}

static inline bool USART_register_driver(USART_driver_t *drv) {
    DEBUG_ASSERT(drv); // Проверка указателей
    DEBUG_ASSERT(drv->port < USART_CNT); // Проверка корректности порта

    if (USART_driver_list[drv->port]) {
        return false; // уже зарегистрирован
    }

    USART_driver_list[drv->port] = drv;
    return true;
}

static inline bool USART_load_config(const USART_config_t *cfg) {
    DEBUG_ASSERT(cfg); // Проверка указателей

    const uint32_t usart = USART_get_port_address(cfg->port); // Получение адреса порта

    // Отключение USART и прерываний
    usart_disable_tx_interrupt(usart);
    usart_disable_rx_interrupt(usart);
    usart_disable(usart);

    // Настройка портов и тактирования
    switch (cfg->port) {
        case USART_1: {
            rcc_periph_clock_enable(RCC_USART1);

            if (cfg->remap) {
                rcc_periph_clock_enable(RCC_AFIO);
                gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON, AFIO_MAPR_USART1_REMAP);

                rcc_periph_clock_enable(RCC_GPIOB);
                gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO6);
                gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO7);
                gpio_set(GPIOB, GPIO7);
            } else {
                rcc_periph_clock_enable(RCC_GPIOA);
                gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO9);
                gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO10);
                gpio_set(GPIOA, GPIO10);
            }
            break;
        }
        case USART_2: {
            rcc_periph_clock_enable(RCC_USART2);
            rcc_periph_clock_enable(RCC_GPIOA);
            gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2);
            gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO3);
            gpio_set(GPIOA, GPIO3);
            break;
        }
        case USART_3: {
            rcc_periph_clock_enable(RCC_USART3);
            rcc_periph_clock_enable(RCC_GPIOB);
            gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO10);
            gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO11);
            gpio_set(GPIOB, GPIO11);
            break;
        }
        default: {
            return false;
        }
    }

    // Настройка параметров обмена
    usart_set_baudrate(usart, cfg->baudrate);
    usart_set_databits(usart, 8);
    usart_set_stopbits(usart, USART_STOPBITS_1);
    usart_set_parity(usart, USART_PARITY_NONE);
    usart_set_mode(usart, USART_MODE_TX_RX);

    // Настройка NVIC
    uint8_t irq = USART_get_IRQ_number(cfg->port);
    nvic_set_priority(irq, 5);
    nvic_enable_irq(irq);

    // Включение только прерывания RX на старте
    usart_enable_rx_interrupt(usart);
    usart_enable(usart);

    return true;
}

bool USART_init(USART_driver_t *drv, const USART_config_t *cfg, void *TX_buff, uint16_t TX_size, void *RX_buff, uint16_t RX_size)
{
    DEBUG_ASSERT(drv && cfg);         // Проверка указателей
    DEBUG_ASSERT(TX_buff && TX_size); // Проверка корректности буфера передачи
    DEBUG_ASSERT(RX_buff && RX_size); // Проверка корректности буфера приема

    drv->port = cfg->port;
    drv->stats.is_enabled = cfg->enable_stats;

    // Сброс статистики
    drv->stats.overrun_errors = 0;
    drv->stats.framing_errors = 0;
    drv->stats.parity_errors = 0;
    drv->stats.noise_errors = 0;
    drv->stats.tx_buffer_overflows = 0;
    drv->stats.rx_buffer_overflows = 0;
    drv->stats.tx_idle = 0;
    drv->stats.rx_idle = 0;
    drv->stats.isr_calls = 0;
    drv->stats.bytes_received = 0;
    drv->stats.bytes_sent = 0;

    if (!USART_register_driver(drv)) { return false; }  // Регистрация драйвера
    if (!USART_load_config(cfg)) { return false; }      // Загрузка конфигурации

    // Инициализация буферов
    ring_buffer_init(&drv->TX_buff, TX_buff, TX_size, sizeof(uint8_t));
    ring_buffer_init(&drv->RX_buff, RX_buff, RX_size, sizeof(uint8_t));

    return true;
}

bool USART_send(USART_driver_t *drv, uint8_t byte) { // Неблокирующая отправка
    DEBUG_ASSERT(drv);

    if (!ring_buffer_push(&drv->TX_buff, &byte)) {
        if (drv->stats.is_enabled) {
            ++(drv->stats.tx_buffer_overflows);
        }
        return false;
    }

    uint32_t usart_addr = USART_get_port_address(drv->port);
    usart_enable_tx_interrupt(usart_addr);
    return true;
}


bool USART_try_send(USART_driver_t *drv, uint8_t byte, uint32_t timeout_tick) { // Попытка неблокирующей отправки, сброс по таймауту
    DEBUG_ASSERT(drv);

    uint32_t start = g_SysTick_cnt;

    while (!ring_buffer_push(&drv->TX_buff, &byte)) {
        if ((g_SysTick_cnt - start) > timeout_tick) { return false; }
        ++(drv->stats.tx_idle);
        MACRO_ASM_DATA_SYNC_BARRIER;
        MACRO_ASM_WAIT_FOR_INTERRUPT;
    }

    const uint32_t usart = USART_get_port_address(drv->port);
    usart_enable_tx_interrupt(usart);
    return true;
}

bool USART_receive(USART_driver_t *drv, uint8_t *byte) { // Неблокирующий прием
    DEBUG_ASSERT(drv && byte);
    return ring_buffer_pop(&drv->RX_buff, byte);
}

bool USART_try_receive(USART_driver_t *drv, uint8_t *byte, uint32_t timeout_tick) { // Попытка неблокирующего приема, сброс по таймеру
    DEBUG_ASSERT(drv && byte);

    uint32_t start = g_SysTick_cnt;

    while (!ring_buffer_pop(&drv->RX_buff, byte)) {
        if ((g_SysTick_cnt - start) > timeout_tick) { return false; }
        ++(drv->stats.rx_idle);
        MACRO_ASM_DATA_SYNC_BARRIER;
        MACRO_ASM_WAIT_FOR_INTERRUPT;
    }

    return true;
}

// Обработка прерывания USART
void USART_IRQ_handler(USART_driver_t *drv) {
    DEBUG_ASSERT(drv);

    const uint32_t usart_addr = USART_get_port_address(drv->port); // Получение адреса порта
    const uint32_t sr = USART_SR(usart_addr);

    if(drv->stats.is_enabled) { ++(drv->stats.isr_calls); } // Подсчет прерываний

    if (USART_has_errors(sr)) { // Подсчет ошибок
        volatile uint32_t dummy = USART_DR(usart_addr); (void)dummy; // Сброс DR в случае ошибки

        if (drv->stats.is_enabled) {
            if (sr & USART_SR_ORE) drv->stats.overrun_errors++;
            if (sr & USART_SR_FE) drv->stats.framing_errors++;
            if (sr & USART_SR_NE) drv->stats.noise_errors++;
            if (sr & USART_SR_PE) drv->stats.parity_errors++;
        }
        return;
    }

    if (USART_RX_is_ready(sr)) { // RX: байт/прерывание
        uint8_t byte = usart_recv(usart_addr);

        if (!USART_has_errors(sr)) {
            if (ring_buffer_push(&drv->RX_buff, &byte)) {
                if (drv->stats.is_enabled) { drv->stats.bytes_received++; }

                if (!g_usart_rx_pending[drv->port]) {
                    g_usart_rx_pending[drv->port] = true;
                    //debug_serial_printf("[%u] ITA\n", get_current_time_ms());
                    service_timer_enable();
                }
            }
            else {
                if (drv->stats.is_enabled) { drv->stats.rx_buffer_overflows++; }
            }
        }
    }

    if (USART_TX_is_ready(sr)) { // TX: байт/прерывание
        uint8_t byte;
        if (ring_buffer_pop(&drv->TX_buff, &byte)) {
            usart_send(usart_addr, byte);
            if (drv->stats.is_enabled) { drv->stats.bytes_sent++; }
        } else {
            usart_disable_tx_interrupt(usart_addr);
        }
    }
}

void USART1_Handler() {
    DEBUG_ASSERT(USART_driver_list[USART_1]);
    USART_IRQ_handler(USART_driver_list[USART_1]);
}

void USART2_Handler() {
    DEBUG_ASSERT(USART_driver_list[USART_2]);
    USART_IRQ_handler(USART_driver_list[USART_2]);
}

void USART3_Handler() {
    DEBUG_ASSERT(USART_driver_list[USART_3]);
    USART_IRQ_handler(USART_driver_list[USART_3]);
}