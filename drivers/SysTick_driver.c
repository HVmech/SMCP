#include <drivers/SysTick_driver.h>

#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

volatile uint32_t g_SysTick_cnt = 0;

void SysTick_init(void) {
    // Расчёт количества тактов для 1 мс и источника AHB/8:
    const uint32_t ticks = (SMCP_CONST_RTC_FREQ_MHZ * 1000000U / 8U / 1000U) * CONST_MS_PER_TICK - 1U;

    // Настройка SysTick:
    systick_set_reload(ticks); // Перезагрузка с заданным периодом (ticks)
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8); // Источник тактирования: AHB/8
    nvic_set_priority(NVIC_SYSTICK_IRQ, 3);
    systick_interrupt_enable(); // Разрешение прерывания
    systick_counter_enable(); // Запуск таймера
}

void SysTick_Handler(void) { // Прерывание SysTick
    ++g_SysTick_cnt;
}