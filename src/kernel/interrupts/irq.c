#include "irq.h"
#include "idt.h"
#include "isr.h"
#include "video/console.h"

// PIC constants
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
//#define PIC1_OFFSET 0x20

#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
//#define PIC2_OFFSET 0x28

#define PIC_EOI 0x20

#define PIC_REMAP_OFFSET 0x20

u8 auto_eoi = false;
u16 mask = 0xFFFF;

enum {
    PIC_ICW1_ICW4           = 0x01,
    PIC_ICW1_SINGLE         = 0x02,
    PIC_ICW1_INTERVAL4      = 0x04,
    PIC_ICW1_LEVEL          = 0x08,
    PIC_ICW1_INITIALIZE     = 0x10
} PIC_ICW1;

enum {
    PIC_ICW4_8086           = 0x1,
    PIC_ICW4_AUTO_EOI       = 0x2,
    PIC_ICW4_BUFFER_MASTER  = 0x4,
    PIC_ICW4_BUFFER_SLAVE   = 0x0,
    PIC_ICW4_BUFFERRED      = 0x8,
    PIC_ICW4_SFNM           = 0x10,
} PIC_ICW4;

enum {
    PIC_CMD_END_OF_INTERRUPT    = 0x20,
    PIC_CMD_READ_IRR            = 0x0A,
    PIC_CMD_READ_ISR            = 0x0B,
} PIC_CMD;

static IRQ_handler handlers[16] = { 0 };

static void irq_stub(struct Registers *regs) {
    u32 irq = regs->int_no - PIC_REMAP_OFFSET;

    if (handlers[irq] != NULL) {
        handlers[irq](regs);
    } else {
        cs_writestr("Unhandled IRQ: ");
        cs_writeu32(irq);
        cs_putchar('\n');
    }

    // send EOI
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

void irq_set_mask(u16 new_mask) {
    mask = new_mask;
    outb(PIC1_DATA, mask & 0xFF);
    iowait();
    outb(PIC2_DATA, mask >> 8);
    iowait();
}

static void irq_remap(u8 offset_pic1, u8 offset_pic2) {
    irq_set_mask(mask);

    outb(PIC1_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    iowait();
    outb(PIC2_COMMAND, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    iowait();

    outb(PIC1_DATA, offset_pic1);
    iowait();
    outb(PIC2_DATA, offset_pic2);
    iowait();

    outb(PIC1_DATA, 0x4); // slave at irq2
    iowait();
    outb(PIC2_DATA, 0x2); // cascade identity
    iowait();

    u8 ecw4 = PIC_ICW4_8086;
    if (auto_eoi) ecw4 |= PIC_ICW4_AUTO_EOI;

    outb(PIC1_DATA, ecw4);
    iowait();
    outb(PIC2_DATA, ecw4);
    iowait();

    irq_set_mask(mask);
}

void irq_enable(u32 i) {
    irq_set_mask(mask & ~(1 << i));
}

void irq_disable(u32 i) {
    irq_set_mask(mask | (1 << i));
}

void irq_install(u32 i, IRQ_handler handler) {
    CLI();
    handlers[i] = handler;
    irq_enable(i);
    idt_enable(i + PIC_REMAP_OFFSET);
    STI();
}

void irq_init() {
    irq_remap(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8);

    for (u32 i = 0; i < 16; i++) {
        isr_install(PIC_REMAP_OFFSET + i, irq_stub);
    }
}
