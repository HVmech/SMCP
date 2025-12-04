#pragma once
#include "types.h"

#define PERIPH_BASE      (0x40000000UL)
#define APB2PERIPH_BASE  (PERIPH_BASE + 0x10000UL)
#define GPIOC_BASE       (APB2PERIPH_BASE + 0x1000UL)
#define RCC_BASE         (0x40021000UL)

#define RCC_APB2ENR      (*(volatile uint32_t*)(RCC_BASE + 0x18))

#define TIM1_BASE        (0x40012C00UL)
#define TIM1_CR1         (*(volatile uint32_t*)(TIM1_BASE + 0x00))
#define TIM1_PSC         (*(volatile uint32_t*)(TIM1_BASE + 0x28))
#define TIM1_ARR         (*(volatile uint32_t*)(TIM1_BASE + 0x2C))
#define TIM1_DIER        (*(volatile uint32_t*)(TIM1_BASE + 0x0C))
#define TIM1_SR          (*(volatile uint32_t*)(TIM1_BASE + 0x10))

// NVIC (для включения прерываний)
#define NVIC_ISER0       (*(volatile uint32_t*)0xE000E100UL)

// Структура GPIO
typedef struct {
    volatile uint32_t CRL;   // 0x00
    volatile uint32_t CRH;   // 0x04
    volatile uint32_t IDR;   // 0x08
    volatile uint32_t ODR;   // 0x0C
    volatile uint32_t BSRR;  // 0x10
    volatile uint32_t BRR;   // 0x14
    volatile uint32_t LCKR;  // 0x18
} GPIO_TypeDef;

// Указатели на периферию
#define GPIOC ((GPIO_TypeDef*)GPIOC_BASE)