section .text.prologue
[bits 32]
[extern _entry]
_entry:
    [extern _main]

    sti

    mov esp, stack
    and esp, -16
    push esp

    call _main
    
    cli
    hlt

section .data
align 32
stack_start:
    times 0x4000 db 0x00
stack: