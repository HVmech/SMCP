#ifndef GPIO_H_DOUBLEINCLUDE
#define GPIO_H_DOUBLEINCLUDE

#include "../tools.h"

typedef struct {
	volatile uint32_t CRL;
	volatile uint32_t CRH;	
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t BRR;
	volatile uint32_t LCKR;
} GPIO_struct;

/* base address gpio ports */
#define GPORT_A_BASE (0x40010800)
#define GPORT_B_BASE (0x40010C00)
#define GPORT_C_BASE (0x40011000)
#define GPORT_D_BASE (0x40011400)
#define GPORT_E_BASE (0x40011800)
#define GPORT_F_BASE (0x40011C00)
#define GPORT_G_BASE (0x40012000)


/* base address bit banding area of gpio ports */
#define GPORT_A_BIT_BANDING 0x42210000
#define GPORT_B_BIT_BANDING 0x42218000
#define GPORT_C_BIT_BANDING 0x42220000
#define GPORT_D_BIT_BANDING 0x42228000
#define GPORT_E_BIT_BANDING 0x42230000
#define GPORT_F_BIT_BANDING 0x42238000
#define GPORT_G_BIT_BANDING 0x42240000

#define SHIFTING_ODR_BIT_BANDING 0x180
#define SHIFTING_IDR_BIT_BANDING 0x100

#define PORTA_ODR_BIT_BANDING ((volatile uint32_t*)(GPORT_A_BIT_BANDING + SHIFTING_ODR_BIT_BANDING))
#define PORTB_ODR_BIT_BANDING ((volatile uint32_t*)(GPORT_B_BIT_BANDING + SHIFTING_ODR_BIT_BANDING))
#define PORTC_ODR_BIT_BANDING ((volatile uint32_t*)(GPORT_C_BIT_BANDING + SHIFTING_ODR_BIT_BANDING))

#define PORTA_IDR_BIT_BANDING ((volatile uint32_t*)(GPORT_A_BIT_BANDING + SHIFTING_IDR_BIT_BANDING))
#define PORTB_IDR_BIT_BANDING ((volatile uint32_t*)(GPORT_B_BIT_BANDING + SHIFTING_IDR_BIT_BANDING))
#define PORTC_IDR_BIT_BANDING ((volatile uint32_t*)(GPORT_C_BIT_BANDING + SHIFTING_IDR_BIT_BANDING))

#define GPIOA ((GPIO_struct*)GPORT_A_BASE)
#define GPIOB ((GPIO_struct*)GPORT_B_BASE)
#define GPIOC ((GPIO_struct*)GPORT_C_BASE)
#define GPIOD ((GPIO_struct*)GPORT_D_BASE)
#define GPIOE ((GPIO_struct*)GPORT_E_BASE)
#define GPIOF ((GPIO_struct*)GPORT_F_BASE)
#define GPIOG ((GPIO_struct*)GPORT_G_BASE)


typedef enum {
	GPIO_MODE_INPUT_PULL_UP    = 0b1000, /* but need to set ODR bit */
	GPIO_MODE_INPUT_PULL_DOWN  = 0b1000, /* but need reset ODR bit */
	GPIO_MODE_INPUT_FLOATING   = 0b0100,
	GPIO_MODE_INPUT_ANALOG     = 0b0000,
	GPIO_MODE_OUTPUT_PUSH_PULL_10MHz  = 0b0001,	/* manage by ODR */
	GPIO_MODE_OUTPUT_OPEN_DRAIN_10MHz = 0b0101,	/* manage by ODR */
	GPIO_MODE_OUTPUT_PUSH_PULL_2MHz  = 0b0010,	/* manage by ODR */
	GPIO_MODE_OUTPUT_OPEN_DRAIN_2MHz = 0b0110,	/* manage by ODR */
	GPIO_MODE_OUTPUT_PUSH_PULL_50MHz  = 0b0011,	/* manage by ODR */
	GPIO_MODE_OUTPUT_OPEN_DRAIN_50MHz = 0b0111,	/* manage by ODR */

	/* mode for alternative feathures of gpio */
	AFIO_MODE_OUTPUT_PUSH_PULL_10MHz  = 0b1001,
	AFIO_MODE_OUTPUT_OPEN_DRAIN_10MHz = 0b1101,
	AFIO_MODE_OUTPUT_PUSH_PULL_2MHz  = 0b1010,
	AFIO_MODE_OUTPUT_OPEN_DRAIN_2MHz = 0b1110,
	AFIO_MODE_OUTPUT_PUSH_PULL_50MHz  = 0b1011,
	AFIO_MODE_OUTPUT_OPEN_DRAIN_50MHz = 0b1111
} GPIO_mode;

typedef enum {
	PORT_A,
	PORT_B,
	PORT_C
} GPIO_port;

 /* declaration structures */
 /*
 * the s_pin_ctrl structure is managing structure for working with
 * pin settings.  
 * */
struct s_pin_ctrl {
	uint8_t num;
	GPIO_struct *port;
	volatile uint32_t *odr_bit;
	volatile uint32_t *idr_bit;
};

/* declaration function gpio.c */
struct s_pin_ctrl pin_configure(GPIO_port port, uint8_t  pin,  GPIO_mode  mode);

inline static void enable_pin(const struct s_pin_ctrl *pin)
{
  *(pin->odr_bit) = 1;
}
inline static void disable_pin(const struct s_pin_ctrl *pin)
{
  *(pin->odr_bit) = 0;
}
inline static void switch_pin(const struct s_pin_ctrl *pin)
{
  *(pin->odr_bit) ^= 1;
}
inline static uint8_t read_pin(const struct s_pin_ctrl *pin)
{
	return *(pin->idr_bit);
}

/* Get structure @s_pin_ctrl  and set mode for it
 * @mode supporting GPIO_mode enum */
void pin_mode(const struct s_pin_ctrl *pin, GPIO_mode mode);

#endif
