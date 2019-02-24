global idt_load
extern idtp
idt_load:
    lidt [idtp]
    ret
; In just a few pages in this tutorial, we will add our Interrupt
; Service Routines (ISRs) right here!
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

irq0:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 32
    jmp irq_common_stub
irq1:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 33
    jmp irq_common_stub
irq2:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 34
    jmp irq_common_stub
irq3:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 35
    jmp irq_common_stub
irq4:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 36
    jmp irq_common_stub
irq5:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 37
    jmp irq_common_stub
irq6:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 38
    jmp irq_common_stub
irq7:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 39
    jmp irq_common_stub
irq8:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 40
    jmp irq_common_stub
irq9:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 41
    jmp irq_common_stub
irq10:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 42
    jmp irq_common_stub
irq11:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 43
    jmp irq_common_stub
irq12:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 44
    jmp irq_common_stub
irq13:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 45
    jmp irq_common_stub
irq14:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 46
    jmp irq_common_stub
irq15:
    cli
    push byte 0    ; Note that these don't push an error code on the stack:
                   ; We need to push a dummy error code
    push byte 47
    jmp irq_common_stub
    extern irq_handler
irq_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, irq_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
    
;  0: Divide By Zero Exception

isr0:
    cli
    push byte 0    ; A normal ISR stub that pops a dummy error code to keep a
                   ; uniform stack frame
    push byte 0
    jmp isr_common_stub

;  1: Debug Exception
isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub
    
isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub


;  8: Double Fault Exception (With Error Code!)
isr8:
    cli
    push byte 8        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

isr9:
    cli
    push byte 0
    push byte 9        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr10:
    cli
    push byte 10        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr11:
    cli
    push byte 11        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr12:
    cli
    push byte 12        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr13:
    cli
    push byte 13        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr14:
    cli
    push byte 14        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr15:
    cli
        push byte 0
    push byte 15        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr16:
    cli
        push byte 0
    push byte 16        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr17:
    cli
        push byte 0
    push byte 17        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr18:
    cli
        push byte 0
    push byte 18        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr19:
    cli
        push byte 0
    push byte 19        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr20:
    cli
        push byte 0
    push byte 20        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr21:
    cli
        push byte 0
    push byte 21        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr22:
    cli
        push byte 0
    push byte 22        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr23:
    cli
        push byte 0
    push byte 23        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr24:
    cli
        push byte 0
    push byte 24        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr25:
    cli
        push byte 0
    push byte 25        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr26:
    cli
        push byte 0
    push byte 26        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr27:
    cli
        push byte 0
    push byte 27        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr28:
    cli
        push byte 0
    push byte 28        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr29:
    cli
        push byte 0
    push byte 29        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr30:
    cli
    push byte 0
    push byte 30        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub
isr31:
    cli
    push byte 0
    push byte 31        ; Note that we DON'T push a value on the stack in this one!
                   ; It pushes one already! Use this type of stub for exceptions
                   ; that pop error codes!
    jmp isr_common_stub

                   ; use the correct stubs to handle error codes and push dummies!

; We call a C function in here. We need to let the assembler know
; that '_fault_handler' exists in another file
extern fault_handler
; This is our common ISR stub. It saves the processor state, sets
; up for kernel mode segments, calls the C-level fault handler,
; and finally restores the stack frame.
isr_common_stub:
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
    mov eax, fault_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
