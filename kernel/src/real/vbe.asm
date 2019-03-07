
global vbe_init
global vbe_biosinfo_data
global vbe_modeinfo_data

; Real program info
[bits 16]
%macro find_mode 3
    mov bx, %1
    mov cx, %2
    mov dl, %3
    call vbe_scanmodes
    jnc .found_mode
%endmacro
section .real
vbe_real.start:
vbe_biosinfo:
    push ax
    push bx
    mov ax, 0x4F00
    mov bx, 0
    mov es, bx
    mov di, vbe_biosinfo_data
    int 0x10
    cmp ax, 0x004F ; (ax != 0x004F) = error
    jne .error

    mov bx, 0
    mov ds, bx
    mov si, vbe_biosinfo_data
    pop bx
    pop ax
    ret

.error:
    pop bx
    pop ax
    add sp, 4
    jmp vbe_error

vbe_modeinfo:
    push ax
    push bx
    push cx
    mov cx, ax
    mov ax, 0x4F01
    mov bx, 0
    mov es, bx
    mov di, vbe_modeinfo_data
    int 0x10
    cmp ax, 0x004F
    jne .error

    mov bx, 0
    mov ds, bx
    mov si, vbe_modeinfo_data
    pop cx
    pop bx
    pop ax
    ret

.error:
    pop cx
    pop bx
    pop ax
    add sp, 6
    jmp vbe_error

vbe_setmode:
    push ax
    push bx
    mov bx, ax
    or bx, 0x4000
    mov ax, 0x4F02
    int 0x10
    cmp ax, 0x004F
    jne .error
    pop bx
    pop ax
    ret
.error:
    pop bx
    pop ax
    add sp, 2
    jmp vbe_error

vbe_init:
    call vbe_biosinfo
.find_preferredmodes:
    ;find_mode 1920, 1080, 32
    ;find_mode 1920, 1080, 24
    ;find_mode 1366, 768, 32
    ;find_mode 1366, 768, 24
    find_mode 1024, 768, 32
    find_mode 1024, 768, 24
    ;find_mode 800, 600, 32
    jmp vbe_error
.found_mode:
    call vbe_setmode
    call vbe_modeinfo
    ret

vbe_scanmodes:
    mov bp, vbe_biosinfo_data

    cld
    mov si, word [bp+14]
    mov ax, word [bp+16]
    mov ds, ax
.mode_info:
    lodsw
    cmp ax, 0xFFFF
    je .end
    mov bp, si
    call vbe_modeinfo ; si = modeinfo
    cmp word [si+18], bx
    jne .next_mode
    cmp word [si+20], cx
    jne .next_mode
    cmp byte [si+25], dl
    jne .next_mode
    clc
    ret

.next_mode:
    mov si, bp
    jmp .mode_info

.end:
    stc
    ret

vbe_error:
    xor ax, ax
    xor bx, bx
    ret

vbe_real.end:

section .real_data
vbe_biosinfo_data: 
    resb 512
vbe_modeinfo_data: 
    resb 256