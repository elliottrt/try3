[bits 16]
[org 0x7C00]
[extern _start]
_start:
 
    KERNEL_LOCATION equ 0x1000
    mov [drive_num], dl

    xor ax, ax
    mov es, ax
    mov ds, ax
    mov sp, 0xFFF0
    ; was 0x9000
    mov bp, sp

    call disk_chk_ext

    call disk_get_info

    mov ax, 0x1 ; start at 1
    mov bx, KERNEL_LOCATION ; put it here
    mov cx, 0x80 ; 64 sectors
    call disk_read

%ifdef CONSOLE_VIDEO_MODE
    ; graphics!!
    mov ah, 0x00
    mov al, 0x13; 0x0D; 0x09
    int 0x10
%endif

    cli ; remove interrupts

    lgdt [GDT_desc] ; load desc table

    ; set last bit of cr0 to 1 for 32 bit mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEG:startprot


drive_num: dw 0
 
%include "src/boot/gdt.S"
%include "src/boot/print.S"
%include "src/boot/disk.S"

; %include "gdt.S"
; %include "print.S"
; %include "disk.S"

[bits 32]
startprot:

    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; mov ebp, 0x90000    ; 32 bit stack base pointer
    ; mov esp, ebp

    jmp KERNEL_LOCATION * 0x10

times 510 - ($-$$) db 0
dw 0xAA55
