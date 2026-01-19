#pragma once

#ifndef SMCP_BOARD_H
#define SMCP_BOARD_H

#include <libopencm3/stm32/gpio.h>
#include <stdint.h>

/*
 * Файл board.h для платы Blue Pill (STM32F103C8T6/STM32F103CCT6)
 *
 * Этот файл содержит таблицу соответствия физических пинов платы
 * и пинов микроконтроллера, чтобы упростить разработку и сделать код более читаемым.
 */

// Структура для описания пина платы
typedef struct {
    uint32_t port;    // Порт микроконтроллера (GPIOA, GPIOB, GPIOC)
    uint16_t pin;     // Пин микроконтроллера (GPIO0, GPIO1, ..., GPIO15)
} s_board_pin_info;

// Таблица соответствия физических пинов платы и пинов микроконтроллера
// Индекс в таблице соответствует номеру физического пина на плате
// Пины, отсутствующие на плате, помечены как {0, 0}
static const s_board_pin_info board_pins_map[] = {
    // Индекс 0 - не используется (нет физического пина 0)
    {0, 0}, // [0] - не используется

    // [1] VBAT - питание, не GPIO
    {0, 0}, // [1] - не GPIO

    // [2] PC13 - TAMPER RTC - PC13LED (встроенный светодиод)
    {GPIOC, GPIO13}, // [2] PC13

    // [3] PC14 - OSC32 IN
    {GPIOC, GPIO14}, // [3] PC14

    // [4] PC15 - OSC32 OUT
    {GPIOC, GPIO15}, // [4] PC15

    // [5] - не подключен
    {0, 0}, // [5] - не используется

    // [6] - не подключен
    {0, 0}, // [6] - не используется

    // [7] NRST - RESET BUTTON
    {0, 0}, // [7] - не GPIO (RESET)

    // [8] - не подключен
    {0, 0}, // [8] - не используется

    // [9] - не подключен
    {0, 0}, // [9] - не используется

    // [10] PA0 - ADC0 -CTS2 - T2C1E - WKUP
    {GPIOA, GPIO0}, // [10] PA0

    // [11] PA1 - ADC1 - RTS2 - T2C2
    {GPIOA, GPIO1}, // [11] PA1

    // [12] PA2 - ADC2 - TX2 - T2C3
    {GPIOA, GPIO2}, // [12] PA2

    // [13] PA3 - ADC3 - RX2 - T2C4
    {GPIOA, GPIO3}, // [13] PA3

    // [14] PA4 - ADC4 - NSS1 - CK2
    {GPIOA, GPIO4}, // [14] PA4

    // [15] PA5 - ADC5 - SCK1
    {GPIOA, GPIO5}, // [15] PA5

    // [16] PA6 - ADC6 - MISO1 - T3C1 - T!BKIN
    {GPIOA, GPIO6}, // [16] PA6

    // [17] PA7 - ADC7 - MOSI1 - T3C2 - T1C1N
    {GPIOA, GPIO7}, // [17] PA7

    // [18] PB0 - ADC8 - T3C3 - T1C2N
    {GPIOB, GPIO0}, // [18] PB0

    // [19] PB1 - ADC9 - T3C4 - T1C3N
    {GPIOB, GPIO1}, // [19] PB1

    // [20] PB2 - BOOT1
    {GPIOB, GPIO2}, // [20] PB2

    // [21] PB10 - SCL2 - TX3 - T2C3N
    {GPIOB, GPIO10}, // [21] PB10

    // [22] PB11 - SDA2 - RX3 - T2C4N
    {GPIOB, GPIO11}, // [22] PB11

    // [23] - не подключен
    {0, 0}, // [23] - не используется

    // [24] - не подключен
    {0, 0}, // [24] - не используется

    // [25] PB12 - SMBAI2 - NSS2 - T1BKIN - CK3
    {GPIOB, GPIO12}, // [25] PB12

    // [26] PB13 - SCK2 - T1C1N - CTS3
    {GPIOB, GPIO13}, // [26] PB13

    // [27] PB14 - MISO2 - T1C2N - RTS3
    {GPIOB, GPIO14}, // [27] PB14

    // [28] PB15 - MOSI2 - T1C3N
    {GPIOB, GPIO15}, // [28] PB15

    // [29] PA8 - CK1 - T1C1 - MCO
    {GPIOA, GPIO8}, // [29] PA8

    // [30] PA9 - TX1 - T1C2
    {GPIOA, GPIO9}, // [30] PA9

    // [31] PA10 - RX1 - T1C3
    {GPIOA, GPIO10}, // [31] PA10

    // [32] PA11 - USB- - CTS1 - T1C4 - CANRX
    {GPIOA, GPIO11}, // [32] PA11

    // [33] PA12 - USB+ - RTS1 - T1ETR - CANTX
    {GPIOA, GPIO12}, // [33] PA12

    // [34] PA13 - JTMS - SWDIO
    {GPIOA, GPIO13}, // [34] PA13

    // [35] - не подключен
    {0, 0}, // [35] - не используется

    // [36] - не подключен
    {0, 0}, // [36] - не используется

    // [37] PA14 - JTCK - SWCLK
    {GPIOA, GPIO14}, // [37] PA14

    // [38] PA15 - JTDI - NSS1 - T2C1E
    {GPIOA, GPIO15}, // [38] PA15

    // [39] PB3 - JTDO - SCK1 - T2C2 - TRACE SWO
    {GPIOB, GPIO3}, // [39] PB3

    // [40] PB4 - JTRST - MISO1 - T3C1
    {GPIOB, GPIO4}, // [40] PB4

    // [41] PB5 - SMBAI1 - MOSI1 - T2C2
    {GPIOB, GPIO5}, // [41] PB5

    // [42] PB6 - SCL1 - T4C1 - TX1
    {GPIOB, GPIO6}, // [42] PB6

    // [43] PB7 - SDA1 - T4C2 - RX1
    {GPIOB, GPIO7}, // [43] PB7

    // [44] BOOT0
    {0, 0}, // [44] - не GPIO (BOOT0)

    // [45] PB8 - T4C3 - SCL1 - CANRX
    {GPIOB, GPIO8}, // [45] PB8

    // [46] PB9 - T4C4 - SDA1 - CANTX
    {GPIOB, GPIO9}, // [46] PB9
};

// Макрос для получения информации о пине по его физическому номеру на плате
#define BOARD_PIN_INFO(pin_num) (board_pins_map[pin_num])

// Макросы для удобства доступа к порту и пину
#define BOARD_PIN_PORT(pin_num) (BOARD_PIN_INFO(pin_num).port)
#define BOARD_PIN_PIN(pin_num)  (BOARD_PIN_INFO(pin_num).pin)

// Специальные пины платы
#define BOARD_PIN_LED_BUILTIN     2   // PC13 - встроенный светодиод
#define BOARD_PIN_BUTTON          10  // PA0 - кнопка (если установлена)

// Пины для периферии (альтернативные функции)
#define BOARD_PIN_USART1_TX       30  // PA9
#define BOARD_PIN_USART1_RX       31  // PA10
#define BOARD_PIN_USART2_TX       12  // PA2
#define BOARD_PIN_USART2_RX       13  // PA3

#define BOARD_PIN_SPI1_SCK        15  // PA5
#define BOARD_PIN_SPI1_MISO       16  // PA6
#define BOARD_PIN_SPI1_MOSI       17  // PA7
#define BOARD_PIN_SPI1_NSS        38  // PA15

#define BOARD_PIN_SPI2_SCK        26  // PB13
#define BOARD_PIN_SPI2_MISO       27  // PB14
#define BOARD_PIN_SPI2_MOSI       28  // PB15
#define BOARD_PIN_SPI2_NSS        25  // PB12

#define BOARD_PIN_I2C1_SCL        42  // PB6
#define BOARD_PIN_I2C1_SDA        43  // PB7

#define BOARD_PIN_I2C2_SCL        21  // PB10
#define BOARD_PIN_I2C2_SDA        22  // PB11

#endif // SMCP_BOARD_H
