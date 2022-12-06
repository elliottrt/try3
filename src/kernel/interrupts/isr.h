#ifndef _try3_isr_h
#define _try3_isr_h

#include "util.h"

typedef void (*ISR_handler)(struct Registers*);

void isr_install(u8 i, ISR_handler handler);
void isr_init();

#endif
