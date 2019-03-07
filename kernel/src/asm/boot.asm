
global main

section .bss
stack:
.bottom:
    resb 65536
.top:

extern kernel_main
section .text
main:
    ; Disable interrupts
    cli
    ; Enable FPU.
    finit
    ; Enable SSE.
    mov rax, cr0
    and al, 0xFB
    or al, 0x2
    mov cr0, rax
    mov rax, cr4
    or ax, 3 << 9
    mov cr4, rax
    ; Set up the stack
    mov rsp, stack.top
    ; Run the kernel
    call kernel_main

    cli ; Disable interrupts
    hlt ; Halt the CPU