extern irq_handler

%macro irqx_handler 1
global irq%1
irq%1:
	cli
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rbp
	push rdi
	push rsi
	push rdx
	push rcx
	push rbx
	push rax
	mov rdi, %1
	jmp irq_common_stub
%endmacro

irqx_handler 0
irqx_handler 1
irqx_handler 2
irqx_handler 3
irqx_handler 4
irqx_handler 5
irqx_handler 6
irqx_handler 7
irqx_handler 8
irqx_handler 9
irqx_handler 10
irqx_handler 11
irqx_handler 12
irqx_handler 13
irqx_handler 14
irqx_handler 15

irq_common_stub:
	call irq_handler
	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rsi
	pop rdi
	pop rbp
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	iretq