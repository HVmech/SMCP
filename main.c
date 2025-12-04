#include "common/registers.h"

void TIM1_UP_Handler(void) {
    TIM1_SR = 0;
    GPIOC->ODR ^= (1U << 13);
}

int main(void) {
    RCC_APB2ENR |= (1U << 4);
    RCC_APB2ENR |= (1U << 11);

    GPIOC->CRH &= ~(0xFU << 20);
    GPIOC->CRH |=  (0x2U << 20);

    TIM1_CR1 = 0;
    TIM1_DIER = 0;
    TIM1_SR = 0;

    TIM1_PSC = 7999;
    TIM1_ARR = 999;

    TIM1_DIER |= (1U << 0);

    TIM1_CR1 |= (1U << 0);

    NVIC_ISER0 |= (1U << 25); 

    GPIOC->BSRR = (1U << (13 + 16));

    while (1) {
        __asm("nop");
    }
}