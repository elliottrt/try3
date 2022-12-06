#ifndef _try3_hal_h
#define _try3_hal_h

#include "types.h"

void system_init(void);
u64 system_timer(void);
void system_sleep_ticks(u64 ticks);
void system_sleep_ms(u32 ms);

#endif
