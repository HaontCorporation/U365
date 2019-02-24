; Code for getting register structure.
; Code for getting register structure.
; Returning pointer to it.
; Here's the code.

global get_registers
get_registers:
	pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp   ; Push us the stack
    push eax
    ret