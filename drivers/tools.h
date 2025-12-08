#ifndef TOOLS_H_DOUBLEINCLUDE
#define TOOLS_H_DOUBLEINCLUDE

#define BIT(x) (1U << (x))

/* Code for exclude - stub!  */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
/* End exclude  */

#define RCC_APB2ENR (*(volatile uint32_t*)0x40021018)
#define RCC_APB1ENR (*(volatile uint32_t*)0x4002101c)

typedef enum {
	AFIOEN = (1<<0),
	IOPAEN = (1<<2),
	IOPBEN = (1<<3),
	IOPCEN = (1<<4),
	IOPDEN = (1<<5),
	IOPEEN = (1<<6),
	IOPFEN = (1<<7),
	IOPGEN = (1<<8),
	ADC1   = (1<<9),
	ADC2   = (1<<10),
	TIM1   = (1<<11),
	SPI1   = (1<<12),
	TIM8   = (1<<13),
	USART  = (1<<14),
	ADC3   = (1<<15),
	TIM9   = (1<<19),
	TIM10  = (1<<20),
	TIM11  = (1<<21),
} APB2ENR_mode;


typedef enum {
	TIM2EN = (1<<0),
	TIM3EN = (1<<1),
	TIM4EN = (1<<2),
	TIM5EN = (1<<3),
	TIM6EN = (1<<4),
	TIM7EN = (1<<5),
	TIM12EN = (1<<6),
	TIM13EN = (1<<7),
	TIM14EN = (1<<8),
} APB1ENR_mode;

inline static void apb2_enable(APB2ENR_mode mode)
{
  RCC_APB2ENR |= mode;
}
inline static void apb1_enable(APB1ENR_mode mode)
{
  RCC_APB1ENR |= mode;
}

#define NVIC_ISERO  (*(volatile unsigned int*)0xE000E100)

void serror(char *message);
void strcpy(char *est, const char *src);

#endif
