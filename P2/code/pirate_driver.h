#ifndef PIRATE_DRIVER_H			/* prevent circular inclusions */
#define PIRATE_DRIVER_H			/* by using protection macros */


void print_buf(int l);

void print_buf_hex(int l);

unsigned int init_pirate();
unsigned int close_pirate();

void set_aux_low();
void set_aux_high();

#endif