#include "../tools.h"
#include "gpio.h"

static void set_mode(GPIO_struct *gpio, uint8_t pin, GPIO_mode mode);

struct s_pin_ctrl pin_configure(GPIO_port port, uint8_t pin, GPIO_mode mode)
{
  struct s_pin_ctrl pinx_ctrl;
  pinx_ctrl.num = pin;
  switch(port){
    case PORT_A : {
      apb2_enable(IOPAEN);               
      pinx_ctrl.port = GPIOA;
      pinx_ctrl.odr_bit = PORTA_ODR_BIT_BANDING + pin;
      pinx_ctrl.idr_bit = PORTA_IDR_BIT_BANDING + pin;
      set_mode(GPIOA, pin, mode);
    } break;
    case PORT_B : {
      apb2_enable(IOPBEN);               
      pinx_ctrl.port = GPIOB;
      pinx_ctrl.odr_bit = PORTB_ODR_BIT_BANDING + pin;
      pinx_ctrl.idr_bit = PORTB_IDR_BIT_BANDING + pin;
      set_mode(GPIOB, pin, mode);
    } break;
    case PORT_C : {
      apb2_enable(IOPCEN);               
      pinx_ctrl.port = GPIOC;
      pinx_ctrl.odr_bit = PORTC_ODR_BIT_BANDING + pin;
      pinx_ctrl.idr_bit = PORTC_IDR_BIT_BANDING + pin;
      set_mode(GPIOC, pin, mode);
    } break;
    default: serror("invalid port");
  }
  return pinx_ctrl;
}

static void set_mode(GPIO_struct *gpio, uint8_t pin, GPIO_mode mode)
{
  uint32_t shift = (pin & 0x7) * 4;

  if(pin < 8){
    gpio->CRL &= ~(0xFU << shift);
    gpio->CRL |= ((uint32_t)mode << shift);
  } else {
    gpio->CRH &= ~(0xFU << shift);
    gpio->CRH |= ((uint32_t)mode << shift);
  }
}

void pin_mode(const struct s_pin_ctrl *pin, GPIO_mode mode)
{
  uint32_t shift = (pin->num & 0x7) * 4;

  if(pin->num < 8){
    pin->port->CRL &= ~(0xFU << shift);
    pin->port->CRL |= ((uint32_t)mode << shift);
  } else {
    pin->port->CRH &= ~(0xFU << shift);
    pin->port->CRH |= ((uint32_t)mode << shift);
  }
}
