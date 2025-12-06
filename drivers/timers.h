#ifndef TIMERS_H_DOUBLEINCLUDE
#define TIMERS_H_DOUBLEINCLUDE

#define TIM1_CR1    (*(volatile uint32_t*)0x40012C00)
#define TIM1_PSC    (*(volatile uint32_t*)0x40012C28)
#define TIM1_ARR    (*(volatile uint32_t*)0x40012C2C)
#define TIM1_DIER   (*(volatile uint32_t*)0x40012C0C)
#define TIM1_SR     (*(volatile uint32_t*)0x40012C10)

#define TIM2_CR1    (*(volatile uint32_t*)0x40000000)
#define TIM2_PSC    (*(volatile uint32_t*)0x40000028)
#define TIM2_ARR    (*(volatile uint32_t*)0x4000002C)
#define TIM2_DIER   (*(volatile uint32_t*)0x4000000C)
#define TIM2_SR     (*(volatile uint32_t*)0x40000010)

#endif
