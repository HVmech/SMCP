#pragma once

#ifndef USART_DRIVER_H
#define USART_DRIVER_H

#include <common/ring_buffer.h>

typedef enum { // Порт USART
    USART_1,
    USART_2,
    USART_3,
    USART_CNT
} e_USART_port;

typedef struct { // Конфигурация USART
    e_USART_port port;
    uint32_t baudrate;
    bool remap_pins;
} s_USART_config;

typedef struct { // Статистика работы USART
    uint32_t overrun_errors;
    uint32_t framing_errors;
    uint32_t parity_errors;
    uint32_t noise_errors;
    uint32_t bytes_received;
    uint32_t bytes_sent;
    bool is_enabled;
} s_USART_stats;

typedef struct { // Драйвер USART
    e_USART_port port;
    s_ring_buffer TX_buff;
    s_ring_buffer RX_buff;
    volatile bool TX_active;
    uint32_t baudrate;
    s_USART_stats stats;
} s_USART_driver;

// Инициализация
bool USART_init(s_USART_driver *drv, const s_USART_config *cfg, uint8_t *TX_buff, const uint16_t TX_size, uint8_t *RX_buff, const uint16_t RX_size, bool need_stats);

bool USART_send(s_USART_driver *drv, uint8_t byte); // Отправка
bool USART_receive(s_USART_driver *drv, uint8_t *byte); // Прием

uint16_t USART_send_sequence(s_USART_driver *drv, const uint8_t *data, uint16_t len); // Отправка массива
uint16_t USART_receive_sequence(s_USART_driver *drv, uint8_t *buf, uint16_t max_len); // Прием массива

// Состояние
bool USART_TX_is_busy(s_USART_driver *drv); // Проверка работы
uint16_t USART_TX_pending(s_USART_driver *drv); // Возврат количества элементов в буфере
uint16_t USART_RX_available(s_USART_driver *drv); // Доступность буфера

bool USART_flush(s_USART_driver *drv, uint32_t timeout_tick); // Ожидание передачи
void USART_clear_RX(s_USART_driver *drv); // Очистка буфера приема
void USART_clear_TX(s_USART_driver *drv); // Очистка буфера передачи

#endif // USART_DRIVER_H