global vgafnt_dump
global vgafnt

[bits 16]
section .real
vgafnt_dump:
    mov ax, 0x1130
    mov bh, 0x06
    int 0x10
    
    push es
    pop ds
    mov si, bp

    mov di, 0
    mov es, di
    mov di, vgafnt

    cld
    mov cx, 4096
    rep movsb

    ret

section .real_data
vgafnt: resb 4096
