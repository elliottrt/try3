#include "system.h"
#include "interrupts/irq.h"
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "interrupts/timer.h"
#include "util.h"

#define TIMER_HZ 1000
#define TIMER_MS (1000 / TIMER_HZ)

void system_init(void) {
	idt_init();
    isr_init();
    irq_init();
    timer_init();

    timer_set_hz(TIMER_HZ);

    // Disable the cursor of textmode
    outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

u64 system_timer(void) {
	return timer_get_ticks();
}

void system_sleep_ticks(u64 ticks) {
	u64 end = timer_get_ticks() + ticks;
	while (timer_get_ticks() < end) {
		asm("hlt");
	}
}

void system_sleep_ms(u32 ms) {
	system_sleep_ticks(ms / TIMER_MS);
}
