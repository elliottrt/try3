CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

GDT_start:
    GDT_null:
        dd 0x0
        dd 0x0

    GDT_code:
        dw 0xFFFF ; 4 GB of code
        dw 0x0 ; base address low
        db 0x0 ; base address middle
        db 0b10011010 ; present, priviledge, priviledge, code/data, code/data, conforming, readable, access
        db 0b11001111 ; granularity, size, reserved, available to use, do not use
        db 0x0 ; base address high

    GDT_data:
        dw 0xFFFF ; 4 GB
        dw 0x0 ; 0
        db 0x0 ; 0
        db 0b10010010 ; present, priviledge, priviledge, code/data, expand dir, read/write, accessed
        db 0b11001111 ; granularity, big, reserved, available to use, do not use
        db 0x0

GDT_end:

GDT_desc:
    dw GDT_end - GDT_start - 1
    dd GDT_start
    