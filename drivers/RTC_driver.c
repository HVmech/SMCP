#include <drivers/RTC_driver.h>
#include <libopencm3/stm32/rcc.h>

void RTC_init(void) { // Настройка RTC от HSE 8МГц с преобразованием в 72МГц
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
}