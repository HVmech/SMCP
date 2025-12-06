#pragma once
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <drivers/smcp_gpio.h>

// LED: PC13, active-low
#define LED_PORT SMCP_GPIO_PORT_C
#define LED_PIN  13

// Timer: TIM2
#define LED_TIMER TIM2
#define LED_TIMER_IRQ NVIC_TIM2_IRQ