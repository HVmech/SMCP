#include <common/board.h>
#include <common/debug_assert.h>

static const s_MCU_pin MCU_pins_map[] = { // Внутренняя таблица соответствия физических пинов платы и пинов МК
    {0, 0}, // 0 - PUNUSED
    {0, 0}, // 1 - VBAT
    {GPIOC, GPIO13}, // PC13 = 2
    {GPIOC, GPIO14}, // PC14 = 3
    {GPIOC, GPIO15}, // PC15 = 4
    {0, 0}, // 5
    {0, 0}, // 6
    {0, 0}, // 7 - NRST
    {0, 0}, // 8
    {0, 0}, // 9
    {GPIOA, GPIO0}, // PA0 = 10
    {GPIOA, GPIO1}, // PA1 = 11
    {GPIOA, GPIO2}, // PA2 = 12
    {GPIOA, GPIO3}, // PA3 = 13
    {GPIOA, GPIO4}, // PA4 = 14
    {GPIOA, GPIO5}, // PA5 = 15
    {GPIOA, GPIO6}, // PA6 = 16
    {GPIOA, GPIO7}, // PA7 = 17
    {GPIOB, GPIO0}, // PB0 = 18
    {GPIOB, GPIO1}, // PB1 = 19
    {GPIOB, GPIO2}, // PB2 = 20
    {GPIOB, GPIO10}, // PB10 = 21
    {GPIOB, GPIO11}, // PB11 = 22
    {0, 0}, // 23
    {0, 0}, // 24
    {GPIOB, GPIO12}, // PB12 = 25
    {GPIOB, GPIO13}, // PB13 = 26
    {GPIOB, GPIO14}, // PB14 = 27
    {GPIOB, GPIO15}, // PB15 = 28
    {GPIOA, GPIO8}, // PA8 = 29
    {GPIOA, GPIO9}, // PA9 = 30
    {GPIOA, GPIO10}, // PA10 = 31
    {GPIOA, GPIO11}, // PA11 = 32
    {GPIOA, GPIO12}, // PA12 = 33
    {GPIOA, GPIO13}, // PA13 = 34
    {0, 0}, // 35
    {0, 0}, // 36
    {GPIOA, GPIO14}, // PA14 = 37
    {GPIOA, GPIO15}, // PA15 = 38
    {GPIOB, GPIO3}, // PB3 = 39
    {GPIOB, GPIO4}, // PB4 = 40
    {GPIOB, GPIO5}, // PB5 = 41
    {GPIOB, GPIO6}, // PB6 = 42
    {GPIOB, GPIO7}, // PB7 = 43
    {0, 0}, // 44 - BOOT0
    {GPIOB, GPIO8}, // PB8 = 45
    {GPIOB, GPIO9}, // PB9 = 46
};

s_MCU_pin get_MCU_pin(e_board_pin pin) { // Функция определения пина МК по номеру пина платы
    DEBUG_ASSERT(pin < PCNT); // Проверка на недопустимое значение
    return MCU_pins_map[pin];
}
