#include "main.h"
#include "PORT.H"
#include "outputs.h"



/*********************************************************************
*    LED Output Functions
********************************************************************/

void LED_ON(uint8_t pin)
{
  write_port_bit(LEDS_PORT, pin, CLEAR_BIT);
}

void LED_OFF(uint8_t pin)
{
  write_port_bit(LEDS_PORT, pin, SET_BIT);
}
