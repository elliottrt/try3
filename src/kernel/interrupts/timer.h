#ifndef _try3_timer_h
#define _try3_timer_h

#include "types.h"

void timer_init(void);
u64 timer_get_ticks(void);
void timer_set_hz(u32 hz);

#endif
