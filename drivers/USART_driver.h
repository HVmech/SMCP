#pragma once

#ifndef SMCP_USART_DRIVER_H
#define SMCP_USART_DRIVER_H

#include <common/ring_buffer.h>
#include <common/types.h>

typedef enum {
    USART_1,
    USART_2,
    USART_3,
    USART_CNT
} USART_port_e;

typedef struct {
    USART_port_e port;
    uint32_t baudrate;
    bool remap;
    bool enable_stats;
} USART_config_t;

typedef struct {
    uint8_t overrun_errors;
    uint8_t framing_errors;
    uint8_t parity_errors;
    uint8_t noise_errors;
    uint8_t tx_buffer_overflows;
    uint8_t rx_buffer_overflows;
    uint16_t tx_idle;
    uint16_t rx_idle;
    uint32_t isr_calls;
    uint16_t bytes_received;
    uint32_t bytes_sent;
    bool is_enabled;
} USART_stats_t;

typedef struct {
    USART_port_e port;
    ring_buffer_t TX_buff;
    ring_buffer_t RX_buff;
    USART_stats_t stats;
} USART_driver_t;

bool USART_init(USART_driver_t *drv, const USART_config_t *cfg, void *TX_buff, uint16_t TX_size, void *RX_buff, uint16_t RX_size); // Инициализация

// Передача
bool USART_send(USART_driver_t *drv, uint8_t byte); // неблокирующий
bool USART_try_send(USART_driver_t *drv, uint8_t byte, uint32_t timeout_tick);

// Прием
bool USART_receive(USART_driver_t *drv, uint8_t *byte); // неблокирующий
bool USART_try_receive(USART_driver_t *drv, uint8_t *byte, uint32_t timeout_tick);

#endif // SMCP_USART_DRIVER_H
