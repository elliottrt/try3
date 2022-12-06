#ifndef _try3_irq_h
#define _try3_irq_h

#include "util.h"
#include "isr.h"

typedef void (*IRQ_handler)(struct Registers *);

void irq_enable(u32 i);
void irq_disable(u32 i);

void irq_init(void);
void irq_install(u32 i, IRQ_handler handler);

#endif
