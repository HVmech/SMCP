
#ifndef LCD1_H_FALCON
#define LCD1_H_FALCON

#include <common/types.h>

  /***********************/
 /* FAST CONFIGURATION  */
/***********************/
/* If you need simple input/output functions without special
 * features (for example input char, clear all, pick line) -
 * UNDEF this! It is specific functions. But if you need to 
 * control cursor, change the height of characters, set the 
 * print direction or write to custom symbols - DEFINE IT! */
#define FUNCTION_LOW_LEVEL

/* If you need high level function, such as backspace, lcd_putc
 * or lcd_puts define this macro for adding that tools */
//#define FUNCTION_ADVANCE

#define LCD_ADDRESS 0x27

typedef enum {
	EV_SB,
	EV_ADDR,
	EV_TXE,
	EV_STOP,
	EV_DONE
} i2c_status;

typedef struct {
	i2c_status status;
	uint8_t address;
	uint8_t data[10];
	uint8_t len;
	uint8_t pos;
} i2c_control;


/*******************************************************
 * For fal_queue
 *******************************************************/
#define SIZE_QUEUE 256

typedef enum {
  TASK_READY, 
  TASK_RUN,
  TASK_DONE
} task_status;

typedef void (*ptr_task_fun)(uint32_t param);

typedef struct {
  volatile task_status status; 
  uint32_t param;
  ptr_task_fun func;
} fal_queue_task;

typedef struct {
  uint8_t head;
  uint8_t tail;
  fal_queue_task buf[SIZE_QUEUE];
} fal_queue;


/*******************************************************
 * LCD API
 *******************************************************/

/* Function for initialize i2c and prepare interface to work */
void lcd_init(void(*cb_delay)(uint32_t));

/* Put this function in super cycle. This happens task managment */
int lcd_process(void);

/* When count of delay is complete, this function must be called
 * It is signal task manager, that the delay time has expired */
void lcd_delay_stop(void);

/* Low level function. It is used in the most specific cases */ 
void lcd_write_chunk(uint8_t chunk, uint8_t rs);

/* Low level function. May send command and data
 * @param data: command or data byte
 * @param rs: 1 - data, 0 - command */
void lcd_write_byte(uint8_t data, uint8_t rs);

/* Just init display. Don't forget to define the macro
 * with the correct address: >search LCD_ADDRESS  */
void lcd_init_display(void);

/* Test function for output hello */
void lcd_send_hello(void);

/* Clears all characters on the screen and set cursor to
 * the start cell of first line */
void lcd_clear(void);

/* Set cursor to the start cell of first line */
void lcd_cursor_home(void);

#ifdef FUNCTION_LOW_LEVEL

void lcd_direction_and_all_shift(uint8_t is_dir, uint8_t is_all_shift);
void lcd_display_on(uint8_t is_on);
void lcd_cursor_on(uint8_t is_on);
void lcd_blink_cursor_on(uint8_t is_on);
void lcd_shift_cursor(uint8_t is_right);
void lcd_shift_all(uint8_t is_right);
void lcd_first_line(uint8_t is_first);

/* Function for set position cursor
 * @param line: select line 1 or 2;
 * @param cell: select position from 1  to 16;
 *
 * This function does't forgive mistakes, so pass correct
 * parameters!
 * */
void lcd_set_cursor_place(uint8_t line, uint8_t cell);

/*
 * Functon for customizing symbols
 *
 * @param code: pick code for overwrite (from 0 to 7). There are 
 * 8 customs codes in total if you are in 5x8 dots mode. 
 * @param *bitmap: array of number, that is lines of bit. In 
 * 5x8 dots mode we have 8 numbers whick keeping 5 significant bits.
 * For example chess board 000_1_0_1_0_1 number (0x15)
 *
 * Be carefull! The cursor position is lost!
 */
void lcd_draw_char(uint8_t code, uint8_t *bitmap);
#endif

#ifdef FUNCTION_ADVANCE

#include <miniprintf.h>

void lcd_backspace(void);
void lcd_putc(const char c);
void lcd_puts(const char *s);
int lcd_printf(const char *format, ...);

#endif

#endif
