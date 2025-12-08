#ifndef MATRIX_KEYBOARD_H_DOUBLEIINCLUDE
#define MATRIX_KEYBOARD_H_DOUBLEIINCLUDE

#include "../tools.h"

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

/* The structure store values from status keypad */
struct kpstat { 
  char pressed;
  char code;
}; 

/* Setup driver matrix keyboard. Getting param: callback functions:
 * @btn_down : should implements logic of button press. char - ascii 
 * 		symbol, that was pressed on keyboard
 * @btn_up : shiuld implements logic of button release. char - ascii 
 * 
 * PORTS - hard pointed. PA0-PA7. Arranged of PORTS from 1 to 8 indicated 
 * on the keyboard has that order
 * --------------------------------
 * | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
 * ---------------------------------
 * |PA5|PA4|PA7|PA6|PA3|PA2|PA1|PA0|
 * ---------------------------------
 *  */
void configure_matrix_keyboard(void (*btn_down)(char), void (*btn_up)(char));


#endif
