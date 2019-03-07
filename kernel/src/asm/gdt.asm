
global gdt_flush
extern gdtr

gdt_flush:
    lgdt [gdtr]
    jmp far qword [ptr]
ptr:
    dq .flush
    dw 0x08
.flush:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ret