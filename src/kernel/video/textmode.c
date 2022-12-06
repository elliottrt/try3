#include "textmode.h"
#include "console.h"

#define CHAR_MEM ((u16*)0xB8000)

static cs_color_t active_color = 0x00;
static u8 active_column = 0x00;
static u8 active_row = 0x00;

#define CLEAR_CHAR 0

void tm_setcolor(cs_color_t color) {
    active_color = color;
}

void tm_clear(void) {
    active_row = 0x00;
    active_column = 0x00;
    for (u32 i = 0; i < CS_WIDTH * CS_HEIGHT; i++) {
        CHAR_MEM[i] = tm_entry(CLEAR_CHAR, active_color);
    }
}

void tm_putcharat(char c, cs_color_t color, u8 x, u8 y) {
    CHAR_MEM[y * CS_WIDTH + x] = tm_entry(c, color);
}

void tm_gotolinelast(void) {
    u16 *row = CHAR_MEM + active_row * CS_WIDTH;
    active_column = 0;
    u16 clear_entry = tm_entry(CLEAR_CHAR, active_color);
    while (row[active_column] != clear_entry) active_column++;
}

void tm_putchar(char c) {
    switch (c) {
        case '\r':
        case '\b': {
            if (active_column == 0)  {
                if (active_row == 0) return;
                active_row--;
                tm_gotolinelast();
            }
             else tm_putcharat(CLEAR_CHAR, active_color, --active_column, active_row);
        } break;
        case '\n': {
            active_column = 0;
            active_row++;
        } break;
        default: {
            tm_putcharat(c, active_color, active_column, active_row);
            if (++active_column >= CS_WIDTH) {
                active_column = 0;
                if (++active_row >= CS_HEIGHT)
                    active_row = 0;
            }
        } break;
    }
}

void tm_setchar(char c) {
    tm_putcharat(c, active_color, active_column, active_row);
}

void tm_write(u8 *data, u64 size) {
    for (u64 i = 0; i < size; i++)
        tm_putchar(data[i]);
}

void tm_writestr(const char *str) {
    char c;
    while ((c = *(str++)) != '\0') {
        tm_putchar(c); 
    }
}

void tm_writeu32(u32 n) {
    if (n > 9) tm_writeu32(n/10);
    tm_putchar('0'+ n % 10);
}

static inline char _hex(u8 fbits) {
    return ((const char*) "0123456789ABCDEF") [fbits];
}

void tm_writeu32hex(u32 n) {
    u8 *buf = (u8*) &n;
    tm_putchar('0');
    tm_putchar('x');
    for (i8 i = 3; i >= 0; i--) {
        u8 hi = (buf[i] & 0xF0) >> 4;
        u8 lo = buf[i] & 0x0F;

        tm_putchar(_hex(hi));
        tm_putchar(_hex(lo));
    }
}

void tm_writei32(i32 n) {
    if (n < 0)
    {
       tm_putchar('-');
       n = -n;
    }
    if (n > 9) tm_writei32(n/10);
    tm_putchar('0'+ n%10);
}

void tm_writei32hex(i32 n) {
    u8 *buf = (u8*) &n;
    tm_putchar('0');
    tm_putchar('x');
    for (i8 i = 3; i >= 0; i--) {
        u8 hi = (buf[i] & 0xF0) >> 4;
        u8 lo = buf[i] & 0x0F;

        tm_putchar(_hex(hi));
        tm_putchar(_hex(lo));
    }
}

#define dmpr(N, V) \
    tm_writestr(N ": "); \
    tm_writeu32hex(V); \
    tm_putchar('\n');

void tm_dumpregs(const struct Registers *regs) {
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

void tm_panic(char const *reason) {
    tm_setcolor(0x1F);
    tm_clear();
    tm_writestr(reason);
    while(true);
}

void tm_panic_regs(char const *reason, const struct Registers *regs) {
    tm_setcolor(0x1F);
    tm_clear();
    tm_writestr(reason);
    tm_putchar('\n');
    tm_putchar('\n');
    if (regs != NULL) tm_dumpregs(regs);
    while(true);
}
