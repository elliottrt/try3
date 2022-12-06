#ifndef videomode_h
#define videomode_h

#include "types.h"

#define VWIDTH ((u32)320)
#define VHEIGHT ((u32)200)

#define VM_COLOR(_r, _g, _b)((u8)( \
    (((_r) & 0x7) << 5) |       \
    (((_g) & 0x7) << 2) |       \
    (((_b) & 0x3) << 0)))

void vm_drawpalette(void);

void vm_setpixel(u8 color, u16 x, u16 y);

void vm_drawbitmap(const u8 *bitmap, u16 x, u16 y, u16 width, u16 height, cs_color_t color);

void vm_setcolor(u8 fg, u8 bg);

void vm_clear(void);

void vm_putcharat(char c, cs_color_t color, u8 x, u8 y);

void vm_putchar(char c);

void vm_setchar(char c);

void vm_write(u8 *data, u64 size);

void vm_writestr(const char *str);

void vm_writeu32(u32 n);
void vm_writeu32hex(u32 n);

void vm_writei32(i32 n);
void vm_writei32hex(i32 n);

void vm_dumpregs(const struct Registers *regs);

void vm_panic(char const *reason);
void vm_panic_regs(char const *reason, const struct Registers *regs);

#endif /* videomode_h */
