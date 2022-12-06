[bits 16]

; prints a null terminated string
; address: si
print_string:
    push ax
    push bx
    push si

.loop:

    xor bh, bh
    mov ah, 0xE

    lodsb

    cmp al, 0
    je print_string.end

    int 0x10
    jmp print_string.loop

.end:
    pop si
    pop bx
    pop ax
    ret

; prints a single byte hex in bx
print_hex:
    push cx
    push ax
    push si

    mov cx, bx
    mov si, hex_table
    mov ah, 0x0E

    shr bx, 12
    mov al, [bx + si]     
    int 0x10
                          

    mov bx, cx            
    shr bx, 8             
    and bx, 0x0F          
    mov al, [bx + si]     
    int 0x10

    mov bx, cx
    shr bx, 4
    and bx, 0x0F
    mov al, [si + bx]
    int 0x10

    mov bx, cx
    and bx, 0x0F
    mov al, [si + bx]
    int 0x10

    pop si
    pop ax
    pop cx
    ret

hex_table: db "0123456789ABCDEF"