#include "idt.h"

struct IDTEntry {
    u16 offset_low;
    u16 selector;
    u8 __ignored;
    u8 flags;
    u16 offset_high;
} PACKED;

struct IDTPointer {
    u16 limit;
    uptr base;
} PACKED;

struct IDTEntry entries[256];
struct IDTPointer pointer;

// in interrupts.S
extern void idt_load(struct IDTPointer *idt_ptr);

void idt_set(u8 index, void (*base)(struct Registers), u16 selector, u8 flags) {
    entries[index] = (struct IDTEntry) {
        .offset_low = ((uptr) base) & 0xFFFF,
        .offset_high = (((uptr) base) >> 16) & 0xFFFF,
        .selector = selector,
        .flags = flags,
        .__ignored = 0
    };
}

void idt_enable(u8 index) {
    entries[index].flags |= IDT_FLAG_PRESENT;
}

void idt_disable(u8 index) {
    entries[index].flags &= ~IDT_FLAG_PRESENT;
}

void idt_init() {
    pointer.limit = sizeof(entries) - 1;
    pointer.base = (uptr) entries;
    memset(entries, 0, sizeof(entries));
    idt_load(&pointer);
}
