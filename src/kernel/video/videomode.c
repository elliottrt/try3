#include "videomode.h"
#include "util.h"
#include "font_8x8.h"
#include "console.h"

#define VMEM ((u8*)0xA0000)

#define PALETTE_MASK 0x3C6
#define PALETTE_READ 0x3C7
#define PALETTE_WRITE 0x3C8
#define PALETTE_DATA 0x3C9

static u8 active_fg_color = 0x00;
static u8 active_bg_color = 0x00;
static u8 active_column = 0x00;
static u8 active_row = 0x00;

static char VM_CHARS[CS_WIDTH * CS_HEIGHT] = {0};

#define CLEAR_CHAR 0

/*
void vm_usepalette(void) {
	outb(PALETTE_MASK, 0xFF);
    outb(PALETTE_WRITE, 0);
    for (u8 i = 0; i < 255; i++) {
        outb(PALETTE_DATA, (((i >> 5) & 0x7) * (256 / 8)) / 4);
        outb(PALETTE_DATA, (((i >> 2) & 0x7) * (256 / 8)) / 4);
        outb(PALETTE_DATA, (((i >> 0) & 0x3) * (256 / 4)) / 4);
    }

    // set color 255 = white
    outb(PALETTE_DATA, 0x3F);
    outb(PALETTE_DATA, 0x3F);
    outb(PALETTE_DATA, 0x3F);
}
*/

void vm_drawpalette(void) {
    u8 square[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    u8 color = 0x00;
    for (u32 y = 0; y < 16; y++) {
        for (u32 x = 0; x < 16; x++) {
            vm_drawbitmap(square, x * 8, y * 8, 8, 8, color++);
        }
    }
}

#define FAST_PLOT

void vm_setpixel(u8 color, u16 x, u16 y) {
#ifdef FAST_PLOT
	VMEM[(y<<8) + (y<<6) + x] = color;
#else
	VMEM[y * VWIDTH + x] = color;
#endif
}

void vm_drawbitmap(const u8 *bitmap, u16 x, u16 y, u16 width, u16 height, cs_color_t color) {
    for (u64 yy = 0; yy < height; yy++) {
        for (u64 xx = 0; xx < width; xx++) {
            if (bitmap[yy] & (1 << xx)) {
                vm_setpixel(color, x + xx, y + yy);
            }
        }
    }
}

void vm_setcolor(u8 fg, u8 bg) {
    active_fg_color = fg;
    active_bg_color = bg;
}

void vm_clear(void) {
    active_row = 0x00;
    active_column = 0x00;
	for (u32 i = 0; i < VWIDTH * VHEIGHT; i++) {
		VMEM[i] = active_bg_color;
	}
}

void vm_putcharat(char c, cs_color_t color, u8 x, u8 y) {
    vm_drawbitmap(FONT_8X8[(u32) c], 
                  (u16) x * FONT_8X8_WIDTH, 
                  (u16) y * FONT_8X8_HEIGHT, 
                  FONT_8X8_WIDTH, 
                  FONT_8X8_HEIGHT, 
                  color);
    VM_CHARS[y * CS_WIDTH + x] = c;
}

void vm_gotolinelast(void) {
    char *row = &VM_CHARS[active_row * CS_WIDTH];
    active_column = 0;
    while (row[active_column] != CLEAR_CHAR) active_column++;
}

void vm_putchar(char c) {
    switch (c) {
        case '\r':
        case '\b': {
            if (active_column == 0)  {
                if (active_row == 0) return;
                active_row--;
                vm_gotolinelast();
            }
            else vm_putcharat(CLEAR_CHAR, active_bg_color, --active_column, active_row);
        } break;
        case '\n': {
            active_column = 0;
            active_row++;
        } break;
        default: {
            vm_putcharat(c, active_fg_color, active_column, active_row);
            if (++active_column >= CS_WIDTH) {
                active_column = 0;
                if (++active_row >= CS_HEIGHT)
                    active_row = 0;
            }
        } break;
    }
}

void vm_setchar(char c) {
    vm_putcharat(c, active_fg_color, active_column, active_row);
}

void vm_write(u8 *data, u64 size) {
    for (u64 i = 0; i < size; i++)
        vm_putchar(data[i]);
}

void vm_writestr(const char *str) {
    char c;
    while ((c = *(str++)) != '\0') {
        vm_putchar(c); 
    }
}

void vm_writeu32(u32 n) {
    if (n > 9) vm_writeu32(n/10);
    vm_putchar('0'+ n % 10);
}

static inline char _hex(u8 fbits) {
    return ((const char*) "0123456789ABCDEF") [fbits];
}

void vm_writeu32hex(u32 n) {
    u8 *buf = (u8*) &n;
    vm_putchar('0');
    vm_putchar('x');
    for (i8 i = 3; i >= 0; i--) {
        u8 hi = (buf[i] & 0xF0) >> 4;
        u8 lo = buf[i] & 0x0F;

        vm_putchar(_hex(hi));
        vm_putchar(_hex(lo));
    }
}

void vm_writei32(i32 n) {
    if (n < 0)
    {
       vm_putchar('-');
       n = -n;
    }
    if (n > 9) vm_writei32(n/10);
    vm_putchar('0'+ n%10);
}

void vm_writei32hex(i32 n) {
    u8 *buf = (u8*) &n;
    vm_putchar('0');
    vm_putchar('x');
    for (i8 i = 3; i >= 0; i--) {
        u8 hi = (buf[i] & 0xF0) >> 4;
        u8 lo = buf[i] & 0x0F;

        vm_putchar(_hex(hi));
        vm_putchar(_hex(lo));
    }
}

#define dmpr(N, V) \
    vm_writestr(N ": "); \
    vm_writeu32hex(V); \
    vm_putchar('\n');

void vm_dumpregs(const struct Registers *regs) {
    dmpr("EAX", regs->eax);
    dmpr("EBX", regs->ebx);
    dmpr("ECX", regs->ecx);
    dmpr("EDX", regs->edx);
    dmpr("ESP", regs->esp);
    dmpr("EBP", regs->ebp);
    dmpr("ESI", regs->esi);
    dmpr("EDI", regs->edi);
    dmpr("EIP", regs->eip);
    dmpr("INT", regs->int_no);
    dmpr("ERR", regs->err_no);
}

void vm_panic(char const *reason) {
    vm_setcolor(CS_WHITE, CS_BLUE);
    vm_clear();
    vm_writestr(reason);
    while(true);
}

void vm_panic_regs(char const *reason, const struct Registers *regs) {
    vm_setcolor(CS_WHITE, CS_BLUE);
    vm_clear();
    vm_writestr(reason);
    vm_putchar('\n');
    vm_putchar('\n');
    if (regs != NULL) vm_dumpregs(regs);
    while(true);
}
