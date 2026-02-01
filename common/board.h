#pragma once

#ifndef SMCP_BOARD_H
#define SMCP_BOARD_H

#include <libopencm3/stm32/gpio.h>
#include <common/types.h>

typedef struct { // Структура пина МК
    uint32_t port;
    uint16_t pin;
} s_MCU_pin;

typedef enum { // Нумерованные пины платы
    PUNUSED = 0, // Не используется
    PVCC = 1, // VBAT - питание, не GPIO
    PC13 = 2, // PC13 - TAMPER RTC - PC13 LED
    PC14 = 3, // PC14 - OSC32 IN
    PC15 = 4, // PC15 - OSC32 OUT
    PNC5 = 5, // Не подключен
    PNC6 = 6, // Не подключен
    PNC7 = 7, // NRST - RESET BUTTON
    PNC8 = 8, // Не подключен
    PNC9 = 9, // Не подключен
    PA0 = 10, // PA0 - ADC0 - CTS2 - T2C1E - WKUP
    PA1 = 11, // PA1 - ADC1 - RTS2 - T2C2
    PA2 = 12, // PA2 - ADC2 - TX2 - T2C3
    PA3 = 13, // PA3 - ADC3 - RX2 - T2C4
    PA4 = 14, // PA4 - ADC4 - NSS1 - CK2
    PA5 = 15, // PA5 - ADC5 - SCK1
    PA6 = 16, // PA6 - ADC6 - MISO1 - T3C1 - T1BKIN
    PA7 = 17, // PA7 - ADC7 - MOSI1 - T3C2 - T1C1N
    PB0 = 18, // PB0 - ADC8 - T3C3 - T1C2N
    PB1 = 19, // PB1 - ADC9 - T3C4 - T1C3N
    PB2 = 20, // PB2 - BOOT1
    PB10 = 21, // PB10 - SCL2 - TX3 - T2C3N
    PB11 = 22, // PB11 - SDA2 - RX3 - T2C4N
    PNC23 = 23, // Не подключен
    PNC24 = 24, // Не подключен
    PB12 = 25, // PB12 - SMBAI2 - NSS2 - T1BKIN - CK3
    PB13 = 26, // PB13 - SCK2 - T1C1N - CTS3
    PB14 = 27, // PB14 - MISO2 - T1C2N - RTS3
    PB15 = 28, // PB15 - MOSI2 - T1C3N
    PA8 = 29,  // PA8 - CK1 - T1C1 - MCO
    PA9 = 30,  // PA9 - TX1 - T1C2
    PA10 = 31, // PA10 - RX1 - T1C3
    PA11 = 32, // PA11 - USB- - CTS1 - T1C4 - CANRX
    PA12 = 33, // PA12 - USB+ - RTS1 - T1ETR - CANTX
    PA13 = 34, // PA13 - JTMS - SWDIO
    PNC35 = 35, // Не подключен
    PNC36 = 36, // Не подключен
    PA14 = 37, // PA14 - JTCK - SWCLK
    PA15 = 38, // PA15 - JTDI - NSS1 - T2C1E
    PB3 = 39, // PB3 - JTDO - SCK1 - T2C2 - TRACE SWO
    PB4 = 40, // PB4 - JTRST - MISO1 - T3C1
    PB5 = 41, // PB5 - SMBAI1 - MOSI1 - T2C2
    PB6 = 42, // PB6 - SCL1 - T4C1 - TX1
    PB7 = 43, // PB7 - SDA1 - T4C2 - RX1
    PBOOT0 = 44, // BOOT0
    PB8 = 45, // PB8 - T4C3 - SCL1 - CANRX
    PB9 = 46, // PB9 - T4C4 - SDA1 - CANTX
    PCNT
} e_board_pin;

// Специальные пины платы
#define PIN_LED_BUILTIN     PC13
#define PIN_WKUP_BUTTON     PA0

#define PIN_USART1_TX       PA9
#define PIN_USART1_RX       PA10
#define PIN_USART2_TX       PA2
#define PIN_USART2_RX       PA3

#define PIN_SPI1_SCK        PA5
#define PIN_SPI1_MISO       PA6
#define PIN_SPI1_MOSI       PA7
#define PIN_SPI1_NSS        PA15

#define PIN_SPI2_SCK        PB13
#define PIN_SPI2_MISO       PB14
#define PIN_SPI2_MOSI       PB15
#define PIN_SPI2_NSS        PB12

#define PIN_I2C1_SCL        PB6
#define PIN_I2C1_SDA        PB7

#define PIN_I2C2_SCL        PB10
#define PIN_I2C2_SDA        PB11

s_MCU_pin get_MCU_pin(e_board_pin pin); // Определение пина МК по пину платы

#endif // SMCP_BOARD_H
