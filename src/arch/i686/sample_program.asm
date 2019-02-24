; Syscall test code. Sample U365 program in x86 assembler.
global syscall_test
hello db 'About this text\nThis text is a output of a first U365 program.\nIt uses system calls on interrupt 0x80 to print text.\n', 0
syscall_test:
	push ebp
	mov ebp, esp
	mov eax, 0
	mov ebx, hello
	int 0x80
	mov esp, ebp
	pop ebp
	ret