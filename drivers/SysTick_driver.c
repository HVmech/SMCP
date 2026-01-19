#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <drivers/SysTick_driver.h>

static volatile uint32_t SysTick_counter = 0;

void SysTick_init(void) {
    // Расчёт количества тактов для 1 мс и источника AHB/8:
    const uint32_t ticks = (SMCP_CONST_RTC_FREQ_MHZ * 1000000U / 8U / 1000U) * CONST_MS_PER_TICK - 1U;

    // Настройка SysTick:
    systick_set_reload(ticks); // Перезагрузка с заданным периодом (ticks)
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8); // Источник тактирования: AHB/8
    systick_interrupt_enable(); // Разрешение прерывания
    systick_counter_enable(); // Запуск таймера
}

inline uint32_t get_SysTick(void) {
    return SysTick_counter;
}

void SysTick_Handler(void) { // Прерывание SysTick
    SysTick_counter++;
}