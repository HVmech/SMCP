#ifndef GPIO_H_DOUBLEINCLUDE
#define GPIO_H_DOUBLEINCLUDE

/* Code for exclude - stub!  */
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
/* End exclude  */

typedef struct {
	/* Register CRL */
	volatile uint32_t MODE0_0;	/* bit_0 */
	volatile uint32_t MODE0_1;	/* bit_1 */
	volatile uint32_t CNF0_0;	/* bit_2 */
	volatile uint32_t CNF0_1;	/* bit_3 */
	volatile uint32_t MODE1_0;	/* bit_4 */
	volatile uint32_t MODE1_1;	/* bit_5 */
	volatile uint32_t CNF1_0;	/* bit_6 */
	volatile uint32_t CNF1_1;	/* bit_7 */
	volatile uint32_t MODE2_0;	/* bit_8 */
	volatile uint32_t MODE2_1;	/* bit_9 */
	volatile uint32_t CNF2_0;	/* bit_10 */
	volatile uint32_t CNF2_1;	/* bit_11 */
	volatile uint32_t MODE3_0;	/* bit_12 */
	volatile uint32_t MODE3_1;	/* bit_13 */
	volatile uint32_t CNF3_0;	/* bit_14 */
	volatile uint32_t CNF3_1;	/* bit_15 */
	volatile uint32_t MODE4_0;	/* bit_16 */
	volatile uint32_t MODE4_1;	/* bit_17 */
	volatile uint32_t CNF4_0;	/* bit_18 */
	volatile uint32_t CNF4_1;	/* bit_19 */
	volatile uint32_t MODE5_0;	/* bit_20 */
	volatile uint32_t MODE5_1;	/* bit_21 */
	volatile uint32_t CNF5_0;	/* bit_22 */
	volatile uint32_t CNF5_1;	/* bit_23 */
	volatile uint32_t MODE6_0;	/* bit_24 */
	volatile uint32_t MODE6_1;	/* bit_25 */
	volatile uint32_t CNF6_0;	/* bit_26 */
	volatile uint32_t CNF6_1;	/* bit_27 */
	volatile uint32_t MODE7_0;	/* bit_28 */
	volatile uint32_t MODE7_1;	/* bit_29 */
	volatile uint32_t CNF7_0;	/* bit_30 */
	volatile uint32_t CNF7_1;	/* bit_31 */

	/* Register CRH */
	volatile uint32_t MODE8_0;	/* bit_0 */
	volatile uint32_t MODE8_1;	/* bit_1 */
	volatile uint32_t CNF8_0;	/* bit_2 */
	volatile uint32_t CNF8_1;	/* bit_3 */
	volatile uint32_t MODE9_0;	/* bit_4 */
	volatile uint32_t MODE9_1;	/* bit_5 */
	volatile uint32_t CNF9_0;	/* bit_6 */
	volatile uint32_t CNF9_1;	/* bit_7 */
	volatile uint32_t MODE10_0;	/* bit_8 */
	volatile uint32_t MODE10_1;	/* bit_9 */
	volatile uint32_t CNF10_0;	/* bit_10 */
	volatile uint32_t CNF10_1;	/* bit_11 */
	volatile uint32_t MODE11_0;	/* bit_12 */
	volatile uint32_t MODE11_1;	/* bit_13 */
	volatile uint32_t CNF11_0;	/* bit_14 */
	volatile uint32_t CNF11_1;	/* bit_15 */
	volatile uint32_t MODE12_0;	/* bit_16 */
	volatile uint32_t MODE12_1;	/* bit_17 */
	volatile uint32_t CNF12_0;	/* bit_18 */
	volatile uint32_t CNF12_1;	/* bit_19 */
	volatile uint32_t MODE13_0;	/* bit_20 */
	volatile uint32_t MODE13_1;	/* bit_21 */
	volatile uint32_t CNF13_0;	/* bit_22 */
	volatile uint32_t CNF13_1;	/* bit_23 */
	volatile uint32_t MODE14_0;	/* bit_24 */
	volatile uint32_t MODE14_1;	/* bit_25 */
	volatile uint32_t CNF14_0;	/* bit_26 */
	volatile uint32_t CNF14_1;	/* bit_27 */
	volatile uint32_t MODE15_0;	/* bit_28 */
	volatile uint32_t MODE15_1;	/* bit_29 */
	volatile uint32_t CNF15_0;	/* bit_30 */
	volatile uint32_t CNF15_1;	/* bit_31 */

	/* Register IDR */
	volatile uint32_t IDR0;		/* bit_0 */
	volatile uint32_t IDR1;		/* bit_1 */
	volatile uint32_t IDR2;		/* bit_2 */
	volatile uint32_t IDR3;		/* bit_3 */
	volatile uint32_t IDR4;		/* bit_4 */
	volatile uint32_t IDR5;		/* bit_5 */
	volatile uint32_t IDR6;		/* bit_6 */
	volatile uint32_t IDR7;		/* bit_7 */
	volatile uint32_t IDR8;		/* bit_8 */
	volatile uint32_t IDR9;		/* bit_9 */
	volatile uint32_t IDR10;	/* bit_10 */
	volatile uint32_t IDR11;	/* bit_11 */
	volatile uint32_t IDR12;	/* bit_12 */
	volatile uint32_t IDR13;	/* bit_13 */
	volatile uint32_t IDR14;	/* bit_14 */
	volatile uint32_t IDR15;	/* bit_15 */

	volatile uint32_t reserved_idr[16];

	/* Register ODR */
	volatile uint32_t ODR0;		/* bit_0 */
	volatile uint32_t ODR1;		/* bit_1 */
	volatile uint32_t ODR2;		/* bit_2 */
	volatile uint32_t ODR3;		/* bit_3 */
	volatile uint32_t ODR4;		/* bit_4 */
	volatile uint32_t ODR5;		/* bit_5 */
	volatile uint32_t ODR6;		/* bit_6 */
	volatile uint32_t ODR7;		/* bit_7 */
	volatile uint32_t ODR8;		/* bit_8 */
	volatile uint32_t ODR9;		/* bit_9 */
	volatile uint32_t ODR10;	/* bit_10 */
	volatile uint32_t ODR11;	/* bit_11 */
	volatile uint32_t ODR12;	/* bit_12 */
	volatile uint32_t ODR13;	/* bit_13 */
	volatile uint32_t ODR14;	/* bit_14 */
	volatile uint32_t ODR15;	/* bit_15 */

	volatile uint32_t reserved_odr[16];
} GPIO_struct_bb;

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

#define GPIOA_BB ((GPIO_struct_bb*)GPORT_A_BIT_BANDING)
#define GPIOB_BB ((GPIO_struct_bb*)GPORT_B_BIT_BANDING)
#define GPIOC_BB ((GPIO_struct_bb*)GPORT_C_BIT_BANDING)
#define GPIOD_BB ((GPIO_struct_bb*)GPORT_D_BIT_BANDING)
#define GPIOE_BB ((GPIO_struct_bb*)GPORT_E_BIT_BANDING)
#define GPIOF_BB ((GPIO_struct_bb*)GPORT_F_BIT_BANDING)
#define GPIOG_BB ((GPIO_struct_bb*)GPORT_G_BIT_BANDING)


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

/* Get structure @s_pin_ctrl  and set mode for it
 * @mode supporting GPIO_mode enum */
void pin_mode(struct s_pin_ctrl *pin, GPIO_mode mode);

#endif
