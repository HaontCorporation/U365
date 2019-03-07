
global exec_real
extern vbe_init

section .real_stack
real_stack: 
    resb 4096
real_stack.end:

section .text
exec_real: use64
    mov word [entry_point], di

    mov qword [saved_stack], rsp
    
    sidt [saved_idt]
    lidt [ivt]
    
    mov rax, cr3
    mov qword [saved_cr3], rax

    jmp far dword [ptr16]

ptr16:
    dd real_runtime
    dw 0x28

section .real
real_runtime:
.pmode16: use16
    mov ax, 0x30
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov eax, cr0
    and eax, 0x7FFFFFFE
    mov cr0, eax

    mov ecx, 0xC0000080
    rdmsr
    and eax, 0xFFFFFEFF
    wrmsr

    jmp 0:.rmode16

.rmode16: use16
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, real_stack.end

    sti
    call word [entry_point]
    cli

    mov eax, 0x9000
    mov cr3, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x00000100
    wrmsr

    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax

    ; NOTE: don't switch to compat mode while selectors are 0

    jmp 0x08:lmode64
.end: ; End of runtime

lmode64: use64
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    lidt [saved_idt]
    mov rsp, qword [saved_stack]

    ret

section .real_data
ivt: 
    dw 0x3FF
    dq 0
saved_idt:
    dw 0 
    dq 0
entry_point: dw 0
saved_stack: dq 0
saved_cr3: dq 0

section .text