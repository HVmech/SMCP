#include "gpio.h"
#include "../tools.h"
#include "matrix_keyboard.h"

/* Structure for store value. rel @kpstat (matrix_keyboard.h) */
struct kpstat keyboard_state;

/* This ports are settings for output push pull  */
const struct s_pin_ctrl rowsport[4] = {
  {0, GPIOA, PORTA_ODR_BIT_BANDING, PORTA_IDR_BIT_BANDING},
  {1, GPIOA, PORTA_ODR_BIT_BANDING + 1, PORTA_IDR_BIT_BANDING + 1},
  {2, GPIOA, PORTA_ODR_BIT_BANDING + 2, PORTA_IDR_BIT_BANDING + 2},
  {3, GPIOA, PORTA_ODR_BIT_BANDING + 3, PORTA_IDR_BIT_BANDING + 3}
};

/* This ports - for input. Them values we will be read  */
const struct s_pin_ctrl colsport[4] = {
  {4, GPIOA, PORTA_ODR_BIT_BANDING + 4, PORTA_IDR_BIT_BANDING + 4},
  {5, GPIOA, PORTA_ODR_BIT_BANDING + 5, PORTA_IDR_BIT_BANDING + 5},
  {6, GPIOA, PORTA_ODR_BIT_BANDING + 6, PORTA_IDR_BIT_BANDING + 6},
  {7, GPIOA, PORTA_ODR_BIT_BANDING + 7, PORTA_IDR_BIT_BANDING + 7}
};

/* Table of chars symbols from keyboard. All elements are arranged so
 * that they can be getting by index is equal to the received code @code */
char keys[] = {
  '3','6','9','#','A','B','C','D',
  '1','4','7','*','2','5','8','0'};

/* Pointers for access into interrupt handlers */
void (*btn_down_callback)(char);
void (*btn_up_callback)(char);

/* Enable timer TIM2 for periodic interruptions
 * and checking state of keypad */
static void configure_and_start_cheking_tim2()
{
  /* Configure TIM2  */
  TIM2_PSC = 3599;
  TIM2_ARR = 99;
  TIM2_DIER |= 1;                       /* enable interrupt TIM2 */
  /* Enable interrupt in NVIC */
  NVIC_ISERO |= (1<<28);
  /* Start timer! */
  TIM2_CR1 |= (1<<0);                   /* set CEN bit. Start timer2 */
}


/* Configure ports, timer, enable clocking and interrupts.
 * Call this function in your module, getting pointers on event 
 * handler functions. */
void configure_matrix_keyboard(void (*btn_down)(char), void (*btn_up)(char)){
  /* Bindings pointers callback functions. They will be using for 
   * sending message about event from keyboard. That functions are
   * designed to implement logic of events. */
  btn_down_callback = btn_down;
  btn_up_callback = btn_up;

  /* Enable clocking for GPIOA */ 
  apb2_enable(IOPAEN);

  /* Configure all modes for ports */
  for(int i = 0; i<4 ; i++){ 
    pin_mode(&rowsport[i], GPIO_MODE_OUTPUT_PUSH_PULL_2MHz);
    pin_mode(&colsport[i], GPIO_MODE_INPUT_PULL_UP);

    /* For mode input pull up - setting bits ODR */
    enable_pin(&colsport[i]);
  }

  configure_and_start_cheking_tim2();
}

/* Interrupt handler */
void timer2_up_handler(void) __attribute__((used));
void timer2_up_handler(void)
{
  /* Reset flag of interrupt - UIF */
  TIM2_SR = 0;  

  int code = -1;

  /* Pooling. Start cheking, row by row, all inputs - cols 
   * Firstly reset one row, cheking input value all 
   * columns and next for each until we detect reset input. Then
   * handling of result */
  for(int i = 0; i < 4 ; i++){
    disable_pin(&(rowsport[i])); 
    for(int j = 0; j < 4; j++){
      if(read_pin(&(colsport[j]))){
        continue;
      }    

      if(keyboard_state.pressed){
        return;
      }
      /* Form value of code @code. This code important for 
       * indexing table with chars related it */
      code = (j << 2) | i;

      keyboard_state.pressed = 1; /* set value into global structure */
      keyboard_state.code = code;
      btn_down_callback(keys[code]);
    }
    enable_pin(&(rowsport[i]));
  }
  if(code == -1 && keyboard_state.pressed){
    keyboard_state.pressed = 0; 
    btn_up_callback(keys[(int)keyboard_state.code]);
  }
}

