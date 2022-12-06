#include "timer.h"
#include "util.h"
#include "irq.h"

static u64 timer_ticks = 0;

void timer_interrupt(struct Registers *regs) {
	timer_ticks++;
}

void timer_init(void) {
	irq_install(0, timer_interrupt);
}

u64 timer_get_ticks(void) {
	return timer_ticks;
}

void timer_set_hz(u32 hz) {
    u32 divisor = 1193180 / hz;       /* Calculate our divisor */
    outb(0x43, 0x36);             /* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outb(0x40, divisor >> 8);     /* Set high byte of divisor */
}
