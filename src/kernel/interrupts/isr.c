#include "isr.h"
#include "idt.h"
#include "video/console.h"

#include "isr_stubs.h"

#define NUM_ISRS 256

static const char *exceptions[32] = {
    "Divide by zero",
    "Debug",
    "NMI",
    "Breakpoint",
    "Overflow",
    "OOB",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor segment overrun",
    "Bad TSS",
    "Segment not present",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unrecognized interrupt",
    "Coprocessor fault",
    "Alignment check",
    "Machine check",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED"
};

static ISR_handler handlers[NUM_ISRS] = { 0 };

void isr_install(u8 i, ISR_handler handler) {
    handlers[i] = handler;
}

// referenced from interrupts.S
void isr_handler(struct Registers *regs) {
    if (handlers[regs->int_no]) {
        handlers[regs->int_no](regs);
    } else {
        cs_writestr("Hit unhandled interrupt ");
        cs_writeu32(regs->int_no);
        cs_putchar('\n');
    }
}

static void exception_handler(struct Registers *regs) {
    cs_panicregs(exceptions[regs->int_no], regs);
}

void isr_init() {
    isr_fill_stubs();

    for (u32 i = 0; i < 32; i++) {
        isr_install(i, exception_handler);
        idt_enable(i);
    }
}
