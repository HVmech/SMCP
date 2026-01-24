#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/cortex.h>
#include <drivers/USART_driver.h>
#include <common/macro_asm.h>
#include <common/macro_debug.h>

static const uint32_t USART_port_map[] = {USART1, USART2, USART3};
static const uint32_t USART_IRQ_map[] = {NVIC_USART1_IRQ, NVIC_USART2_IRQ, NVIC_USART3_IRQ};

static s_USART_driver* USART_driver_list[USART_CNT];

static inline void USART_register_driver(s_USART_driver *drv) {
    DEBUG_STATIC_CHECK_FALSE(drv);
    DEBUG_STATIC_CHECK_FALSE(drv->port < USART_CNT);
    USART_driver_list[drv->port] = drv;
}

static inline uint32_t get_USART_port_address(e_USART_port port) { // Получение адреса USART
    DEBUG_STATIC_CHECK_FALSE_RET(port < USART_CNT, 0);
    return USART_port_map[port];
}

static inline uint8_t get_USART_IRQ_number(e_USART_port port) { // Получение номера прерывания
    DEBUG_STATIC_CHECK_FALSE_RET(port < USART_CNT, 0);
    return USART_IRQ_map[port];
}

static inline void USART_TX_start_transmission(s_USART_driver *drv) {
    DEBUG_STATIC_CHECK_FALSE(drv);
    if (drv->TX_active) { return; } // Передача уже запущена
    
    __asm volatile ("cpsid i");
    //cm_disable_interrupts(); // Выключение прерываний
    if (!ring_buffer_is_empty(&drv->TX_buff)) {
        uint8_t data;
        ring_buffer_read(&drv->TX_buff, &data);
        USART_DR(get_USART_port_address(drv->port)) = data;
        drv->TX_active = true;
        USART_CR1(get_USART_port_address(drv->port)) |= USART_CR1_TXEIE;
    }
    __asm volatile ("cpsie i");
    //cm_enable_interrupts(); // Включение прерываний
}

bool USART_init(s_USART_driver *drv, const s_USART_config *cfg, uint8_t *TX_buff, const uint16_t TX_size, uint8_t *RX_buff, const uint16_t RX_size, bool need_stats) {
    DEBUG_STATIC_CHECK_FALSE_RET(drv, false);
    DEBUG_STATIC_CHECK_FALSE_RET(cfg, false);
    drv->port = cfg->port;
    drv->baudrate = cfg->baudrate;
    drv->TX_active = false;
    drv->stats.is_enabled = need_stats;

    USART_register_driver(drv);

    if (TX_buff && TX_size) { // Инициализация буфера передачи
        ring_buffer_init(&drv->TX_buff, TX_buff, TX_size);
    }
    
    if (RX_buff && RX_size) { // Инициализация буфера приема
        ring_buffer_init(&drv->RX_buff, RX_buff, RX_size);
    }

    switch (drv->port) { // Аппаратная настройка USART
        case USART_1: {
            rcc_periph_clock_enable(RCC_USART1);
            rcc_periph_clock_enable(RCC_AFIO);
            if (cfg->remap_pins) { // Альтернативные пины USART1: PB6, PB7
                gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON, AFIO_MAPR_USART1_REMAP);
                rcc_periph_clock_enable(RCC_GPIOB);
                gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO6);
                gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO7);
                gpio_set(GPIOB, GPIO7);
            } else { // Стандартные пины USART1: PA9, PA10
                rcc_periph_clock_enable(RCC_GPIOA);
                gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO9);
                gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO10);
                gpio_set(GPIOA, GPIO10);
            }
            break;
        }
        case USART_2: { // Стандартные пины USART2: PA2/PA3
            rcc_periph_clock_enable(RCC_USART2);
            rcc_periph_clock_enable(RCC_GPIOA);
            gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2);
            gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO3);
            gpio_set(GPIOA, GPIO3);
            break;
        }
        case USART_3: { // Стандартные пины USART3: PB10/PB11
            rcc_periph_clock_enable(RCC_USART3);
            rcc_periph_clock_enable(RCC_GPIOB);
            gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO10);
            gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO11);
            gpio_set(GPIOB, GPIO11);
            break;
        }
        default: { return false; }
    }
    
    const uint32_t USART_adress = get_USART_port_address(drv->port);
    usart_set_baudrate(USART_adress, drv->baudrate);
    usart_set_databits(USART_adress, 8);
    usart_set_stopbits(USART_adress, USART_STOPBITS_1);
    usart_set_parity(USART_adress, USART_PARITY_NONE);
    usart_set_mode(USART_adress, USART_MODE_TX_RX);

    if (RX_buff) { usart_enable_rx_interrupt(USART_adress); } // Включение прерывания приема при наличии буфера
    usart_disable_tx_interrupt(USART_adress); // Выключение прерывания передачи

    // Настройка контроллера прерываний NVIC
    const uint8_t USART_number = get_USART_IRQ_number(drv->port);
    nvic_set_priority(USART_number, 1);
    nvic_enable_irq(USART_number);
    
    usart_enable(USART_adress);
    return true;
}

bool USART_send(s_USART_driver *drv, uint8_t byte) {
    DEBUG_STATIC_CHECK_FALSE_RET(drv, false);
    if (!ring_buffer_write(&drv->TX_buff, byte)) return false;
    USART_TX_start_transmission(drv);
    return true;
}

bool USART_receive(s_USART_driver *drv, uint8_t *byte) {
    DEBUG_STATIC_CHECK_FALSE_RET(drv, false);
    DEBUG_STATIC_CHECK_FALSE_RET(byte, false);
    return ring_buffer_read(&drv->RX_buff, byte);
}

uint16_t USART_send_sequence(s_USART_driver *drv, const uint8_t *data, uint16_t len) {
    DEBUG_STATIC_CHECK_FALSE_RET(drv, 0);
    DEBUG_STATIC_CHECK_FALSE_RET(data, 0);
    DEBUG_STATIC_CHECK_FALSE_RET(len, 0);
    uint16_t sent = 0;
    while (sent < len && USART_send(drv, data[sent])) {
        sent++;
    }
    return sent;
}

uint16_t USART_receive_sequence(s_USART_driver *drv, uint8_t *buf, uint16_t max_len) {
    DEBUG_STATIC_CHECK_FALSE_RET(drv, 0);
    DEBUG_STATIC_CHECK_FALSE_RET(buf, 0);
    DEBUG_STATIC_CHECK_FALSE_RET(max_len, 0);
    uint16_t read = 0;
    while (read < max_len && USART_receive(drv, &buf[read])) {
        read++;
    }
    return read;
}

bool USART_TX_is_busy(s_USART_driver *drv) {
    DEBUG_STATIC_CHECK_FALSE_RET(drv, false);
    return !ring_buffer_is_empty(&drv->TX_buff) || drv->TX_active;
}

uint16_t USART_TX_pending(s_USART_driver *drv) {
    DEBUG_STATIC_CHECK_FALSE_RET(drv, false);
    return ring_buffer_count(&drv->TX_buff);
}

uint16_t USART_RX_available(s_USART_driver *drv) {
    DEBUG_STATIC_CHECK_FALSE_RET(drv, false);
    return ring_buffer_count(&drv->RX_buff);
}

bool USART_flush(s_USART_driver *drv, uint32_t timeout_tick) {
    DEBUG_STATIC_CHECK_FALSE_RET(drv, false);
    DEBUG_STATIC_CHECK_FALSE_RET(timeout_tick < (uint32_t)-1, false);
    for (uint32_t i = 0; i < timeout_tick; i++) {
        if (!USART_TX_is_busy(drv)) { return true; }
        MACRO_ASM_DO_NOTHING;
    }
    return false;
}

void USART_clear_RX(s_USART_driver *drv) {
    DEBUG_STATIC_CHECK_FALSE(drv);
    ring_buffer_clear(&drv->RX_buff);
}

void USART_clear_TX(s_USART_driver *drv) {
    DEBUG_STATIC_CHECK_FALSE(drv);
    ring_buffer_clear(&drv->TX_buff);
    drv->TX_active = false;
    usart_disable_tx_interrupt(get_USART_port_address(drv->port));
}

static inline bool USART_TX_is_ready(uint32_t status_reg) { // Проверка готовности передачи
    return status_reg & USART_SR_TXE;
}

static inline bool USART_RX_is_ready(uint32_t status_reg) { // Проверка готовности приема
    return status_reg & USART_SR_RXNE;
}

static inline bool USART_TX_interrupt_enabled(uint32_t usart) { // Проверка включения прерывания передачи
    return USART_CR1(usart) & USART_CR1_TXEIE;
}

static inline bool USART_has_errors(uint32_t status_reg) { // Проверка наличия ошибок
    return status_reg & (USART_SR_ORE | USART_SR_NE | USART_SR_FE | USART_SR_PE);
}

static inline void USART_IRQ_handler(s_USART_driver *drv) {
    DEBUG_STATIC_CHECK_FALSE(drv);
    const uint32_t usart_addr = get_USART_port_address(drv->port); // Получение адреса порта
    const uint32_t status_reg = USART_SR(usart_addr); // Чтение регистра статуса
    
    if (USART_has_errors(status_reg)) { // Проверка ошибок
        volatile uint32_t dummy = USART_DR(usart_addr); (void)dummy; // Сброс регистра DR при чтении

        if (drv->stats.is_enabled) { // Логирование ошибок
            if (status_reg & USART_SR_ORE) { drv->stats.overrun_errors++; }
            if (status_reg & USART_SR_FE) { drv->stats.framing_errors++; }
            if (status_reg & USART_SR_PE) { drv->stats.parity_errors++; }
            if (status_reg & USART_SR_NE) { drv->stats.noise_errors++; }
        }

        return;
    }

    if (USART_RX_is_ready(status_reg)) { // Прием
        uint8_t data = usart_recv(usart_addr);
        ring_buffer_write(&drv->RX_buff, data);
        if (drv->stats.is_enabled) { drv->stats.bytes_received++; }
    }
    
    if (USART_TX_is_ready(status_reg) && USART_TX_interrupt_enabled(usart_addr)) { // Передача
        if (!ring_buffer_is_empty(&drv->TX_buff)) {
            uint8_t data;
            ring_buffer_read(&drv->TX_buff, &data);
            usart_send(usart_addr, data);
            if (drv->stats.is_enabled) { drv->stats.bytes_sent++; }
        } else {
            drv->TX_active = false;
            usart_disable_tx_interrupt(usart_addr);
        }
    }
}

void USART1_Handler() {
    DEBUG_STATIC_CHECK_FALSE(USART_driver_list[USART_1]);
    USART_IRQ_handler(USART_driver_list[USART_1]);
}

void USART2_Handler() {
    DEBUG_STATIC_CHECK_FALSE(USART_driver_list[USART_3]);
    USART_IRQ_handler(USART_driver_list[USART_3]);
}

void USART3_Handler() {
    DEBUG_STATIC_CHECK_FALSE(USART_driver_list[USART_3]);
    USART_IRQ_handler(USART_driver_list[USART_3]);
}