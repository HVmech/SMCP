#include "libopencm3/stm32/f1/nvic.h"
#include <drivers/step_timer_driver.h>

#include <common/debug_assert.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

static step_timer_config_t step_cfg;
static bool is_initialized = false;

void step_timer_init(const step_timer_config_t *cfg) {
    DEBUG_ASSERT(cfg);
    step_cfg = *cfg;

    timer_disable_counter(TIM1);
    nvic_disable_irq(NVIC_TIM1_UP_IRQ); // Отключение прерываний по таймеру
    nvic_disable_irq(NVIC_TIM1_BRK_IRQ); // Отключение прерывания остановки
    nvic_set_priority(NVIC_TIM1_UP_IRQ, 2);
    nvic_set_priority(NVIC_TIM1_BRK_IRQ, 1);

    if (!is_initialized) {
        // Включение тактирования GPIO и TIM1
        rcc_periph_clock_enable(RCC_GPIOA);
        rcc_periph_clock_enable(RCC_GPIOB);
        rcc_periph_clock_enable(RCC_TIM1);

        // Настройка вывода
        gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO8);

        gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO12);
        gpio_clear(GPIOB, GPIO12);

        is_initialized = true;
    }

    // Настройка таймера
    timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_prescaler(TIM1, step_cfg.prescaler - 1);
    timer_set_period(TIM1, step_cfg.arr);
    timer_set_repetition_counter(TIM1, step_cfg.rcr); // Настройка частоты прерываний

    // Настройка ШИМ
    timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM2); // Режим генерации испульса при превышении CCR
    timer_set_oc_polarity_low(TIM1, TIM_OC1); // Полярность на старте - LOW
    timer_set_oc_idle_state_set(TIM1, TIM_OC1);
    timer_set_oc_value(TIM1, TIM_OC1, step_cfg.ccr); // Установка ширины импульса

    // Настройка аварийной остановки (BKIN)
    timer_set_break_polarity_high(TIM1); // BKIN активен высоким уровнем
    timer_set_break_lock(TIM1, TIM_BDTR_LOCK_OFF);
    timer_enable_break(TIM1); // Включение функции break

    timer_enable_preload(TIM1);
    timer_enable_oc_preload(TIM1, TIM_OC1);

    timer_enable_oc_output(TIM1, TIM_OC1);
    timer_enable_break_main_output(TIM1);
}

void step_timer_start(void) {
    timer_set_counter(TIM1, 0);
    timer_generate_event(TIM1, TIM_EGR_UG);
    timer_enable_counter(TIM1);
}

void step_timer_stop(void) {
    timer_disable_counter(TIM1);
}

void step_timer_set_period(uint32_t arr_value) {
    timer_set_period(TIM1, arr_value);
    step_timer_set_ccr(arr_value >> 1);
}

void step_timer_set_ccr(uint32_t ccr_value) {
    timer_set_oc_value(TIM1, TIM_OC1, ccr_value);
}

void step_timer_set_rcr(uint16_t rcr_value) {
    timer_set_repetition_counter(TIM1, rcr_value);
}

void step_timer_disable_output(void) {
    timer_disable_oc_output(TIM1, TIM_OC1);
    gpio_clear(GPIOA, GPIO8);
}

void step_timer_enable_irq(void) {
    timer_enable_update_event(TIM1);
    timer_enable_irq(TIM1, TIM_DIER_UIE);
    timer_enable_irq(TIM1, TIM_DIER_BIE);
    nvic_enable_irq(NVIC_TIM1_UP_IRQ);
    nvic_enable_irq(NVIC_TIM1_BRK_IRQ);
}

void step_timer_disable_irq(void) {
    timer_disable_irq(TIM1, TIM_DIER_UIE);
    nvic_disable_irq(NVIC_TIM1_UP_IRQ);
}

void step_timer_reset_update_flag(void) {
    timer_clear_flag(TIM1, TIM_SR_UIF);
}

void step_timer_reset_breakup_flag(void) {
    timer_clear_flag(TIM1, TIM_SR_BIF);
}

void step_timer_update_timer(void) {
    timer_generate_event(TIM1, TIM_EGR_UG);
    timer_clear_flag(TIM1, TIM_SR_UIF);
}