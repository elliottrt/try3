#ifndef _try3_types_h
#define _try3_types_h

typedef char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32;
typedef unsigned int u32;
typedef long long i64;
typedef unsigned long long u64;

typedef u32 uptr;

typedef u8 bool;
#define true 1
#define false 0

#define NULL 0

typedef u8 cs_color_t;

struct Registers {
    u32 ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 int_no, err_no;
    u32 eip, cs, eflags, useresp, ss;
};

#endif /* types_h */
