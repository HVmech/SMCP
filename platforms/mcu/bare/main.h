#ifndef MAIN_STM32
#define MAIN_STM32

#if NUKE_SOKOL_ASS_WITH_MODERN_CPP
using uint32_t = unsigned int;

inline constexpr uint32_t PERIPH_BASE      = 0x40000000UL;
inline constexpr uint32_t APB2PERIPH_BASE  = PERIPH_BASE + 0x10000;
inline constexpr uint32_t GPIOC_BASE       = APB2PERIPH_BASE + 0x1000;

struct GPIO_REGISTER {
    volatile uint32_t CRL;     // 0x00 - Port configuration register low
    volatile uint32_t CRH;     // 0x04 - Port configuration register high  
    volatile uint32_t IDR;     // 0x08 - Port input data register
    volatile uint32_t ODR;     // 0x0C - Port output data register     
    volatile uint32_t BSRR;    // 0x10 - Port bit set/reset register
    volatile uint32_t BRR;     // 0x14 - Port bit reset register
    volatile uint32_t LCKR;    // 0x18 - Port configuration lock register
};

inline auto& GPIOC = *reinterpret_cast<GPIO_REGISTER*>(GPIOC_BASE);
#else
#define NUKE_MY_ASS_WITH_OLD_C_STYLE
    typedef unsigned int uint32_t;

    #define PERIPH_BASE 0x40000000UL
    #define APB2PERIPH_BASE (PERIPH_BASE + 0x10000)
    #define GPIOC_BASE (APB2PERIPH_BASE + 0x1000)

    struct GPIO_TypeDef {
        volatile uint32_t CRL;     // 0x00 - Port configuration register low
        volatile uint32_t CRH;     // 0x04 - Port configuration register high  
        volatile uint32_t IDR;     // 0x08 - Port input data register
        volatile uint32_t ODR;     // 0x0C - Port output data register
        volatile uint32_t BSRR;        // 0x10 - Port bit set/reset register
        volatile uint32_t BRR;     // 0x14 - Port bit reset register
        volatile uint32_t LCKR;    // 0x18 - Port configuration lock register
    };

    #define GPIOC ((GPIO_TypeDef*)GPIOC_BASE)
#endif

// Остатки старой школы:
#define RCC_APB2ENR (*(volatile unsigned int*)0x40021018)

#define TIM1_CR1    (*(volatile unsigned int*)0x40012C00)
#define TIM1_PSC    (*(volatile unsigned int*)0x40012C28)
#define TIM1_ARR    (*(volatile unsigned int*)0x40012C2C)
#define TIM1_DIER   (*(volatile unsigned int*)0x40012C0C)
#define TIM1_SR     (*(volatile unsigned int*)0x40012C10)

#define NVIC_ISERO  (*(volatile unsigned int*)0xE000E100)

#endif