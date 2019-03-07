
extern fault_handler

; isrx_handler: isr number, push dummy code
%macro isrx_handler 2
global isr%1
isr%1:
	cli
	%if %2 == 1
	push qword 0
	%endif
	push qword %1
	jmp isr_common_stub

%endmacro

isrx_handler 0,1
isrx_handler 1,1
isrx_handler 2,1
isrx_handler 3,1
isrx_handler 4,1
isrx_handler 5,1
isrx_handler 6,1
isrx_handler 7,1
isrx_handler 8,0
isrx_handler 9,1
isrx_handler 10,0
isrx_handler 11,0
isrx_handler 12,0
isrx_handler 13,0
isrx_handler 14,0
isrx_handler 15,1 ; Reserved ISR
isrx_handler 16,1
isrx_handler 17,0
isrx_handler 18,1
isrx_handler 19,1
isrx_handler 20,1
isrx_handler 21,1 ; Reserved ISR
isrx_handler 22,1 ;
isrx_handler 23,1 ;
isrx_handler 24,1 ;
isrx_handler 25,1 ;
isrx_handler 26,1 ;
isrx_handler 27,1 ;
isrx_handler 28,1 ;
isrx_handler 29,1 ;
isrx_handler 30,0
isrx_handler 31,1 ; Reserved ISR

isr_common_stub:
	; Push Registers
	push qword gs
	push qword fs
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	mov r8, cr3
	push r8
	mov r8, cr2
	push r8
	mov r8, cr0
	push r8
	push rbp
	push rdi
	push rsi
	push rdx
	push rcx
	push rbx
	push rax

	mov rdi, rsp
	call fault_handler
	
	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rsi
	pop rdi
	pop rbp
	add rsp, 24
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	pop fs
	pop gs
	add rsp, 16
	iret