#ifndef _try3_textmode_h
#define _try3_textmode_h

#include "types.h"

static inline cs_color_t tm_color(u8 fg, u8 bg) {
    return fg | bg << 4;
}
 
static inline u16 tm_entry(char c, cs_color_t color) {
    return (u16) c | (u16) color << 8;
}

void tm_setcolor(cs_color_t color);

void tm_clear(void);

void tm_putcharat(char c, cs_color_t color, u8 x, u8 y);

void tm_putchar(char c);
void tm_setchar(char c);

void tm_write(u8 *data, u64 size);

void tm_writestr(const char *str);

void tm_writeu32(u32 n);
void tm_writeu32hex(u32 n);
void tm_writei32(i32 n);
void tm_writei32hex(i32 n);

void tm_dumpregs(const struct Registers *regs);

void tm_panic(const char *reason);
void tm_panic_regs(const char *reason, const struct Registers *regs);

#endif /* tm_h */
