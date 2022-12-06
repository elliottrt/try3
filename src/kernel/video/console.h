#ifndef _try3_console_h
#define _try3_console_h

#include "types.h"

#define CS_BLACK          0
#define CS_BLUE           1
#define CS_GREEN          2
#define CS_CYAN           3
#define CS_RED            4
#define CS_MAGENTA        5
#define CS_BROWN          6
#define CS_LIGHT_GREY     7
#define CS_DARK_GREY      8
#define CS_LIGHT_BLUE     9
#define CS_LIGHT_GREEN   10
#define CS_LIGHT_CYAN    11
#define CS_LIGHT_RED     12
#define CS_LIGHT_MAGENTA 13
#define CS_LIGHT_BROWN   14
#define CS_WHITE         15

#ifdef CONSOLE_VIDEO_MODE

#include "videomode.h"

#define CS_WIDTH (VWIDTH / 8)
#define CS_HEIGHT (VHEIGHT / 8)

#define cs_init()

#define cs_setcolor(f, b) vm_setcolor(f, b)
#define cs_clear() vm_clear()

#define cs_putcharat(ch, cl, x, y) vm_putcharat(ch, cl, x, y)
#define cs_putchar(c) vm_putchar(c)
#define cs_setchar(c) vm_setchar(c)

#define cs_write(d, s) vm_write(d, s)

#define cs_writestr(s) vm_writestr(s)
#define cs_writeu32(u) vm_writeu32(u)
#define cs_writeu32hex(u) vm_writeu32hex(u)
#define cs_writei32(i) vm_writei32(i)
#define cs_writei32hex(i) vm_writei32hex(i)

#define cs_dumpregs(r) vm_dumpregs(r)

#define cs_panic(r) vm_panic(r)
#define cs_panicregs(ra, re) vm_panic_regs(ra, re)

#endif

#ifdef CONSOLE_TEXT_MODE

#include "textmode.h"

#define CS_WIDTH 80
#define CS_HEIGHT 25

#define cs_init()

#define cs_setcolor(f, b) tm_setcolor((f) | (b) << 4)
#define cs_clear() tm_clear()

#define cs_putcharat(ch, cl, x, y) tm_putcharat(ch, cl, x, y)
#define cs_putchar(c) tm_putchar(c)
#define cs_setchar(c) tm_setchar(c)

#define cs_write(d, s) tm_write(d, s) 

#define cs_writestr(s) tm_writestr(s)
#define cs_writeu32(u) tm_writeu32(u)
#define cs_writeu32hex(u) tm_writeu32hex(u)
#define cs_writei32(i) tm_writei32(u)
#define cs_writei32hex(i) tm_writei32hex(u)

#define cs_dumpregs(r) tm_dumpregs(r)

#define cs_panic(r) tm_panic(r)
#define cs_panicregs(ra, re) tm_panic_regs(ra, re)

#endif

#endif
