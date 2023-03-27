#ifndef RIGOL_H			/* prevent circular inclusions */
#define RIGOL_H			/* by using protection macros */

#include "visa.h"


void rigol_open();
void rigol_close();

int rigol_set_delay_ns(unsigned long delay_ns);

void rigol_out_off();
void rigol_out_on();


#endif