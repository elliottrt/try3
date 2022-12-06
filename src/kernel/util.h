#ifndef _try3_util_h
#define _try3_util_h

#include "types.h"

#ifndef asm
#define asm __asm__ volatile
#endif

#define CLI() asm("cli");
#define STI() asm("sti");

static inline u16 ins(u16 port) {
    u16 r;
    asm("inw %1, %0" : "=a" (r) : "dN" (port));
    return r;
}

static inline void outs(u16 port, u16 data) {
    asm("outw %1, %0" : : "dN" (port), "a" (data));
}

static inline u8 inb(u16 port) {
    u8 r;
    asm("inb %1, %0" : "=a" (r) : "dN" (port));
    return r;
}

static inline void outb(u16 port, u8 data) {
    asm("outb %1, %0" : : "dN" (port), "a" (data));
}

#define UNUSED_PORT 0x80
static inline void iowait(void) {
    outb(UNUSED_PORT, 0x00);
}

#define PACKED __attribute__((packed))

#define HIBIT(_x) (31 - __builtin_clz((_x)))

// returns the lowest set bit of x
#define LOBIT(_x)\
    __extension__({ __typeof__(_x) __x = (_x); HIBIT(__x & -__x); })

// returns _v with _n-th bit = _x
#define BIT_SET(_v, _n, _x) __extension__({\
        __typeof__(_v) __v = (_v);\
        (__v ^ ((-(_x) ^ __v) & (1 << (_n))));\
        })

static u64 strlen(char *str) {
    u64 l = 0;
    while (*str++ != 0) l++;
    return l;
}

static inline void memset(void *dst, u8 value, u64 n) {
    u8 *d = dst;

    while (n-- > 0) {
        *d++ = value;
    }
}

#endif
