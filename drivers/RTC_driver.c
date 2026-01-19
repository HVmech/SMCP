#include <libopencm3/stm32/rcc.h>

void RTC_init(void) { // Настройка RTC от HSE 8МГц с преобразованием в 72МГц
//#if (SMCP_CONST_RTC_FREQ_MHZ == 72U)
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
//#else
//    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_HSI_48MHZ]);
//#endif // SMCP_CONST_RTC_FREQ_MHZ
}