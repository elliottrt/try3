[bits 16]

disk_chk_ext:
    pusha

    stc
    mov ah, 0x41
    mov bx, 0x55AA
    int 0x13

    jnc .done
    mov si, NO_DISK_EXT
    call print_hex
    jmp $
.done:
    popa
    ret


; a structure holding the
; disk information
disk_info:
    .size: dw 0x001E
    .flags: dw 0x0
    .cylinders: dd 0x0
    .heads: dd 0x0
    .sectors: dd 0x0
    .total_sectors: dq 0x0
    .bytes: dw 0x0


; gets and saves disk info
; dl: drive num
disk_get_info:
    pusha

    mov ah, 0x48
    mov si, disk_info
    int 0x13

    jc disk_error

    popa
    ret


; a packet holding info
; for loading blocks
disk_packet:
    .size: db 0x10
    .resr: db 0x00
    .blocks: dw 0x0000
    .offset: dw 0x0000
    .segment: dw 0x0000
    .start: dq 0x0000

; reads cx blocks from block ax
; and stores them at bx
; dl: drive num
disk_read:
    pusha

    mov [disk_packet.blocks], cx
    mov [disk_packet.segment], bx
    mov [disk_packet.start], ax

    mov ah, 0x42
    mov si, disk_packet
    int 0x13

    jc disk_error

    popa
    ret


; resets the disk
; dl - drive number
disk_reset:
    pusha

    stc
    xor ah, ah
    int 0x13

    jc disk_error

    popa
    ret


; prints the DISK_ERR message and the error code
disk_error:
    mov si, DISK_ERR
    call print_string
    xor bh, bh
    mov bl, ah
    call print_hex
    jmp $

%ifdef COMMENT

disk_info_dump:
    pusha

    mov si, DISK_INFO_SECTORS
    call print_string

    mov bx, word [disk_info.total_sectors + 6]
    call print_hex

    mov bx, word [disk_info.total_sectors + 4]
    call print_hex

    mov bx, word [disk_info.total_sectors + 2]
    call print_hex

    mov bx, word [disk_info.total_sectors]
    call print_hex

    mov si, DISK_INFO_BYTES
    call print_string

    mov bx, [disk_info.bytes]
    call print_hex

    popa
    ret

DISK_INFO_SECTORS: db "Sectors:", 0x0
DISK_INFO_BYTES: db  0xA, 0xD, "Bytes/Sector:", 0x0

%endif

DISK_ERR: db "Disk Error:", 0
DISK_SUCCESS:  db "Disk Success", 0
NO_DISK_EXT: db "Err: no exts", 0
