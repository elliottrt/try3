[bits 16]

disk_info:
    .cylinders:
        db 0x0
    .cylinderhi:
        db 0x0
    .sectors:
        dw 0x0
    .heads:
        dw 0x0
    .drives:
        db 0x0


; load the disk's info - http://www.ctyme.com/intr/rb-0621.htm
; in: 
; dl: drive
disk_get_info:
    pusha
    push di
    push es

    stc
    mov ah, 0x8
    xor di, di
    int 0x13

    cmp ah, 0x00
    je .save

    xor bh, bh
    mov bl, ah
    call print_hex
    jmp $
.save:
    mov [disk_info.drives], dl
    inc dh ; dl = number of heads - 1, so we add one
           ; to get number of heads
    mov [disk_info.heads], dh

    mov [disk_info.cylinders], ch

    mov bh, cl
    and cl, 0x3F ; get lower 6 bits
    mov [disk_info.sectors], cl

    mov cl, bh
    and cl, 0xC0 ; get upper 2 bits
    shr cl, 6
    mov [disk_info.cylinderhi], cl
.done:
    pop es
    pop di
    popa
    ret


; converts an LBA stored in ax to CHS
; ch: cylinder
; dh: head
; cl: sector
lba_to_chs:
    push ax
    push dx

    xor dx, dx ; zero dx
    div word [disk_info.sectors] ; ax = LBA / sectors per track
                                 ; dx = LBA % sectors per track
    inc dx ; dx = (LBA % sectors per track) + 1
    mov cx, dx

    xor dx, dx
    div word [disk_info.heads] ; ax = (LBA / sectors per track) / heads
                               ; dx = (LBA / sectors per track) % heads
    mov dh, dl
    mov ch, al
    shl ah, 6
    or cl, ah

    pop ax
    mov dl, al
    pop ax
    ret


disk_read:

    pusha
    push di
    push si
    
    push cx
    call lba_to_chs
    pop ax

    mov ah, 0x2
    mov di, 3 ; retry count
.retry:
    push ax

    stc
    int 0x13

    mov bl, ah
    mov bh, ah
    call print_hex

    pop ax

    jnc .done 

    call disk_reset
    dec di
    test di, di
    jnz .retry
.fail:
    jmp disk_error
.done:

    mov si, DISK_SUCCESS
    call print_string

    pop si
    pop di
    popa
    ret


; load DH sectors to ES:BX from drive DL
; From PDF, 3.6.4

; Kernel Loading
; mov bx, KERNEL_LOCATION ; load stuff at es:bx which is 0x0000:0x1000
; Load 32 sectors
; mov dh, 32
; Load drive_num
; mov dl, [drive_num]
; call the disk loading
; call disk_load

%ifdef COMMENT

disk_load:
    pusha
    
    mov ah, 0x2 ; BIOS read sector function
    mov al, dh  ; Read DH sectors
    mov ch, 0x0 ; Cylinder 0
    mov dh, 0x0 ; Head 0
    mov cl, 0x2 ; Sector 2

    stc
    int 0x13

    ; if the operation failed, error
    jc disk_error

    ; inform the user of a success
    mov si, DISK_SUCCESS
    call print_string

    popa
    ret

%endif