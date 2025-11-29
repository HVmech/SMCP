#include "main.h"

void delay(void) {
    for(volatile int i = 0; i < 1000000; i++);
}

int main(void)
{
  RCC_APB2ENR |= (1 << 4) | (1 << 11);

  #if defined(NUKE_SOKOL_ASS_WITH_MODERN_CPP)
    GPIOC.CRH &= ~(0xF << 20);
    GPIOC.CRH |= (0x6 << 20);
  #else
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |= (0x6 << 20);
  #endif

  TIM1_PSC = 7199;
  TIM1_ARR = 199;
  TIM1_DIER |= 1;

  NVIC_ISERO = (1 << 25);

  TIM1_CR1 |= (1 << 0);
  while(1) {}
}

extern "C" {
  void TIM1_UP_Handler(void) __attribute__((used));

  void TIM1_UP_Handler(void)
  {
    TIM1_SR = 0;

  #if defined(NUKE_SOKOL_ASS_WITH_MODERN_CPP)
    GPIOC.ODR ^= (1 << 13);
  #else
    GPIOC->ODR ^= (1 << 13);
  #endif

  }
}