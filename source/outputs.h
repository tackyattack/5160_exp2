#ifndef _OUTPUTS_H
#define _OUTPUTS_H
#include "Main.h"
#include "PORT.h"

#define CLEAR_BIT (0)
#define SET_BIT   (1)

// ------ Public function prototypes -------------------------------

void write_port_bit(uint8_t port_num, uint8_t bit_num, uint8_t bit_val);
void write_port_value(uint8_t port_num, uint8_t port_val);

#endif