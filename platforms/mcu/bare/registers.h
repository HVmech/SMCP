#pragma once

#include "../../../common/types.h"

inline constexpr t_UINT32 PERIPH_BASE      = 0x40000000UL;
inline constexpr t_UINT32 APB2PERIPH_BASE  = PERIPH_BASE + 0x10000;
inline constexpr t_UINT32 GPIOC_BASE       = APB2PERIPH_BASE + 0x1000;

struct GPIO_REGISTER {
    volatile t_UINT32 CRL;     // 0x00 - Port configuration register low
    volatile t_UINT32 CRH;     // 0x04 - Port configuration register high  
    volatile t_UINT32 IDR;     // 0x08 - Port input data register
    volatile t_UINT32 ODR;     // 0x0C - Port output data register     
    volatile t_UINT32 BSRR;    // 0x10 - Port bit set/reset register
    volatile t_UINT32 BRR;     // 0x14 - Port bit reset register
    volatile t_UINT32 LCKR;    // 0x18 - Port configuration lock register
};

inline auto& GPIOC = *reinterpret_cast<GPIO_REGISTER*>(GPIOC_BASE);

// Остатки старой школы:
#define RCC_APB2ENR (*(volatile t_UINT32*)0x40021018)

#define TIM1_CR1    (*(volatile t_UINT32*)0x40012C00)
#define TIM1_PSC    (*(volatile t_UINT32*)0x40012C28)
#define TIM1_ARR    (*(volatile t_UINT32*)0x40012C2C)
#define TIM1_DIER   (*(volatile t_UINT32*)0x40012C0C)
#define TIM1_SR     (*(volatile t_UINT32*)0x40012C10)

#define NVIC_ISERO  (*(volatile t_UINT32*)0xE000E100)