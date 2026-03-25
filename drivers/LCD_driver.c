#include <drivers/LCD_driver.h>

#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

//#include "usbcdc.h"    
//#include "lcd1.h"

/*
 *  Pins:
 *    I2C1_SCL PB6
 *    I2C1_SDA PB7 
 */

/****************************************************
 * Implementation queue structure 
 ****************************************************/
static void fal_queue_init(fal_queue *q)
{
  q->head = 0;  
  q->tail = 0;
}

static int fal_queue_is_empty(fal_queue *q)
{
  return q->head == q->tail;
}

static int fal_queue_is_full(fal_queue *q)
{
  return ((q->head + 1) % SIZE_QUEUE) == q->tail;
}

static int fal_queue_push(fal_queue *q, ptr_task_fun fun, uint32_t param)
{
  if(fal_queue_is_full(q)){
    return -1;
  }
  fal_queue_task *ntask = &(q->buf[q->head]);

  ntask->status = TASK_READY;
  ntask->func = fun;
  ntask->param = param;

  q->head = (q->head + 1) % SIZE_QUEUE;
  return 0;
}

static int fal_queue_pop(fal_queue *q, fal_queue_task *el)
{
  if(fal_queue_is_empty(q)){
    return -1;
  } 
  if(el != NULL){
    *el = q->buf[q->tail];
  }
  q->tail = (q->tail + 1) % SIZE_QUEUE;
  return 0;
}


/****************************************************
 * Implementation i2c interface for LCD
 ****************************************************/
i2c_control i2c2_ctrl;          /* descriptor I2C2 interface */
fal_queue task_queue;

void (*lcd_cb_delay)(uint32_t ms);  /* Callback for start delay */

static void i2c2_setup(void)
{
  rcc_periph_clock_enable(RCC_I2C2);

  gpio_set_mode(GPIOB,
      GPIO_MODE_OUTPUT_50_MHZ,
      GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
      GPIO_I2C2_SCL | GPIO_I2C2_SDA);

  i2c_peripheral_disable(I2C2);

  i2c_enable_ack(I2C2);
  i2c_set_clock_frequency(I2C2, 36); // APB1 max clocking = 36MHz
  i2c_set_ccr(I2C2, 180);
  i2c_set_trise(I2C2, 36);

  nvic_set_priority(NVIC_I2C2_EV_IRQ, 6);

  /* enable interrupts */
  i2c_enable_interrupt(I2C2, I2C_CR2_ITEVTEN);
  i2c_enable_interrupt(I2C2, I2C_CR2_ITBUFEN);

  /* enable nvic interrupts */
  nvic_enable_irq(NVIC_I2C2_EV_IRQ);
  nvic_enable_irq(NVIC_I2C2_ER_IRQ);

  i2c_peripheral_enable(I2C2);
}


static void i2c_task_delay(uint32_t ms)
{
  /* Callback function for signal start delay.
   * When delay is stop, user call function 
   * @func lcd_delay_stop() */
  lcd_cb_delay(ms);
}

static void i2c_delay(uint32_t ms)
{
  fal_queue_push(&task_queue, i2c_task_delay, ms);
}

static void i2c_task_write(uint32_t byte)
{
  
  i2c2_ctrl.status = EV_SB;
  i2c2_ctrl.pos = 0;
  i2c2_ctrl.len = 1;
  i2c2_ctrl.data[0] = (uint8_t)byte;

  I2C_CR1(I2C2) |= I2C_CR1_START;

  (task_queue.buf[task_queue.tail]).status = TASK_RUN;
}

static void i2c_send_byte(uint8_t byte)
{
  if(fal_queue_is_full(&task_queue)){
    return;
  }
  fal_queue_push(&task_queue, i2c_task_write, (uint32_t)byte);
}

inline static void i2c_delay_stop(void)
{
  (task_queue.buf[task_queue.tail]).status = TASK_DONE;
}

static void i2c2_fsm(void)
{
  switch(i2c2_ctrl.status){
    case EV_SB : {
      if(I2C_SR1(I2C2) & I2C_SR1_SB){
        I2C_DR(I2C2) = i2c2_ctrl.address;
        i2c2_ctrl.status = EV_ADDR;
        gpio_toggle(GPIOC, GPIO13);
      }  
    }
    break;
    case EV_ADDR : {
      if(I2C_SR1(I2C2) & I2C_SR1_ADDR){
        (void)I2C_SR2(I2C2);
        i2c2_ctrl.status = EV_TXE;
        gpio_toggle(GPIOC, GPIO13);
      }
    }
    break;
    case EV_TXE : {
      if((I2C_SR1(I2C2) & (I2C_SR1_TxE | I2C_SR1_BTF)) 
          == (I2C_SR1_TxE & I2C_SR1_BTF)){
        i2c2_ctrl.status = EV_STOP;
      }
      if(I2C_SR1(I2C2) & I2C_SR1_TxE){
        if(i2c2_ctrl.pos == i2c2_ctrl.len){
          i2c2_ctrl.status = EV_STOP;
          return;
        }
        I2C_DR(I2C2) = i2c2_ctrl.data[i2c2_ctrl.pos];
        i2c2_ctrl.pos++;
        gpio_toggle(GPIOC, GPIO13);
      }
    }
    break;
    case EV_STOP : {
      I2C_CR1(I2C2) |= I2C_CR1_STOP; 
      i2c2_ctrl.status = EV_DONE;
      (task_queue.buf[task_queue.tail]).status = TASK_DONE;
    }
    break;
    case EV_DONE : {
      return;
    }
  }
}

//void i2c2_ev_isr()
void I2C2_EV_Handler()
{
  i2c2_fsm();
}

//void i2c2_er_isr()
void I2C2_ER_Handler()
{
  /* TODO handed error! */
  //usb_printf("[DEBUG] i2c2_er_isr()\n");
}

/*************************************************************
 * LCD communicaion code
 *************************************************************/

int lcd_process(void)
{
  if(fal_queue_is_empty(&task_queue)){
    return 1; 
  }
  fal_queue_task *task = &(task_queue.buf[task_queue.tail]);
  
  //ptr_task_fun er_fun = task->func;

  switch(task->status){
    case TASK_READY : {
      //task->status = TASK_RUN;
      task->func(task->param);
      return 0;
    }
    break;
    case TASK_RUN : {
      return 0;
    }
    break;
    case TASK_DONE : {
      fal_queue_pop(&task_queue, NULL);
      return 0;
    }
  } 
  return 2;
}


void lcd_delay_stop(void)
{
  i2c_delay_stop();
}

void lcd_write_chunk(uint8_t chunk, uint8_t rs)
{
  rs &= 0x1;
  uint8_t byte = (chunk & 0xF0) | 0x8 | rs;  
  /* Set bit E = 1 and write */
  i2c_send_byte(byte|0x4);
  //i2c_delay(2);
  /* write wiith E = 0 */
  i2c_send_byte(byte);
  //i2c_delay(2);
}

void lcd_write_byte(uint8_t data, uint8_t rs)
{
  lcd_write_chunk(data, rs);  
  lcd_write_chunk(data<<4, rs);
  i2c_delay(4);
}

void lcd_init_display(void)
{
  i2c2_ctrl.address = LCD_ADDRESS << 1;
  i2c_delay(20); 
  lcd_write_chunk(0x30, 0);
  i2c_delay(20); 
  lcd_write_chunk(0x30, 0);
  i2c_delay(5); 
  lcd_write_chunk(0x30, 0);
  i2c_delay(5); 
  lcd_write_chunk(0x20, 0);
  i2c_delay(5); 

  lcd_write_byte(0x28, 0);        /*  4-bit, 2 line, 5x8 */
  lcd_write_byte(0x0C, 0);        /* display on, cursor off */
  lcd_write_byte(0x06, 0);        /* entry mode: increment */
  lcd_write_byte(0x01, 0);
  i2c_delay(10);
}

void lcd_send_hello(void)
{
  lcd_write_byte(0x80, 0);
  char *h = "Sokol krasavec!!";
  while(*h){
    lcd_write_byte(*h, 1);        /* RS=1 -> data */
    h++;
  }
}

void lcd_clear(void)
{
  lcd_write_byte(0x01, 0); 
}

/* Just set cursor in first cell of first line */
void lcd_cursor_home(void)
{
  lcd_write_byte(0x02, 0);
}

#ifdef FUNCTION_LOW_LEVEL

void lcd_direction_and_all_shift(uint8_t is_dir, uint8_t is_all_shift)
{
  uint8_t byte = 0x04;
  if(is_dir){
    byte |= 0x02;
  }
  if(is_all_shift){
    byte |= 0x01;
  }
  lcd_write_byte(byte, 0);
}


/*
 * Set of changing display hidden, cursor and blinking
 */
static uint8_t state_dcb = 0x08 | 0x04; /* display on by  default */

void lcd_display_on(uint8_t is_on)
{
  if(is_on){
    state_dcb |= 0x04;
  } else {
    state_dcb &= ~0x04; 
  }
  lcd_write_byte(state_dcb, 0);
}

void lcd_cursor_on(uint8_t is_on)
{
  if(is_on){
    state_dcb |= 0x02;
  } else {
    state_dcb &= ~0x02; 
  }
  lcd_write_byte(state_dcb, 0);
}

void lcd_blink_cursor_on(uint8_t is_on)
{
  if(is_on){
    state_dcb |= 0x01;
  } else {
    state_dcb &= ~0x01;
  }
  lcd_write_byte(state_dcb, 0);
}

/*
 *  Shifting cursor or all with cursor
 */
void lcd_shift_cursor(uint8_t is_right)
{
  if(is_right){
    lcd_write_byte(0x14, 0);
  } else {
    lcd_write_byte(0x10, 0);
  }
}

void lcd_shift_all(uint8_t is_right)
{
  if(is_right){
    lcd_write_byte(0x1C, 0);
  } else {
    lcd_write_byte(0x18, 0);
  }
}

void lcd_first_line(uint8_t is_first)
{
  if(is_first){
    lcd_write_byte(0x24, 0);
  } else {
    lcd_write_byte(0x28, 0);
  }
}

/*
 * @param line: select line 1 or 2;
 * @param cell: select position from 1  to 16;
 */
void lcd_set_cursor_place(uint8_t line, uint8_t cell)
{
  uint8_t byte = 0x80;
  if(line != 1){
    byte |= 0x40;     /* Set AC6 bit for go to 2 line */ 
  }
  byte |= (0x0F & (cell-1));

  lcd_write_byte(byte, 0);
}

/*
 * Functon for customizing symbols
 *
 * @param code: pick code for overwrite (from 0 to 7). There are 
 * 8 customs codes in total if you are in 5x8 dots mode. 
 * @param *bitmap: array of number, that is lines of bit. In 
 * 5x8 dots mode we have 8 numbers whick keeping 5 significant bits.
 * For example chess board 000_1_0_1_0_1 number (0x15)
 */
void lcd_draw_char(uint8_t code, uint8_t *bitmap)
{
  uint8_t addr = 0x40 + 8 * code;  
  for(int i = 0; i < 8; i++){
    lcd_write_byte(addr + i, 0);      /* set address CGRAM */
    lcd_write_byte(bitmap[i], 1);      /* write to CGRAM data */
  }
}
#endif

#ifdef FUNCTION_ADVANCE
void lcd_backspace(void)
{
  lcd_write_byte(0x10, 0);  
  lcd_write_byte(0x14, 1);
  lcd_write_byte(0x10, 0);
}

/* Function output char. Be carefull with line length 
 * and line briaks. If display full - look at lcd_clear()  */
void lcd_putc(const char c)
{
  if(c == '\n'){
    /* The carriage is always moved to start second line */
    lcd_write_byte(0xC0, 0);
    return;
  }
  lcd_write_byte(c, 1);
}

void lcd_puts(const char *s)
{
  for(int i = 0; s[i]; i++){
    lcd_putc(s[i]);      
  }
}

int lcd_printf(const char *format, ...)
{
  int rc;
  va_list args;
  va_start(args, format);
  rc = mini_vprintf_cooked(lcd_putc, format, args);
  va_end(args);
  return rc;
}
#endif

void lcd_init(void(*cb_delay)(uint32_t))
{
  i2c2_setup();
  fal_queue_init(&task_queue);
  lcd_cb_delay = cb_delay; 
}



