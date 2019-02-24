; Reading CPU registers in C code.
; 'global' is means that our function can be called from everywhere.
; General-purpose registers first
global read_eax
read_eax:
	push ebp
	mov ebp, esp
	mov eax, eax
	mov esp, ebp
	pop ebp
	ret
global read_ebx
read_ebx:
	push ebp
	mov ebp, esp
	mov eax, ebx
	mov esp, ebp
	pop ebp
	ret
global read_ecx
read_ecx:
	push ebp
	mov ebp, esp
	mov eax, ecx
	mov esp, ebp
	pop ebp
	ret
global read_edx
read_edx:
	push ebp
	mov ebp, esp
	mov eax, edx
	mov esp, ebp
	pop ebp
	ret
global read_esi
read_esi:
	push ebp
	mov ebp, esp
	mov eax, esi
	mov esp, ebp
	pop ebp
	ret
global read_edi
read_edi:
	push ebp
	mov ebp, esp
	mov eax, edi
	mov esp, ebp
	pop ebp
	ret
; Then some control registers like cr*, EFLAGS and other
global read_cr0
read_cr0:
	push ebp
	mov ebp, esp
	mov eax, cr0
	mov esp, ebp
	pop ebp
	ret
global read_cr1
read_cr1:
	push ebp
	mov ebp, esp
	mov eax, cr1
	mov esp, ebp
	pop ebp
	ret
global read_cr2
read_cr2:
	push ebp
	mov ebp, esp
	mov eax, cr2
	mov esp, ebp
	pop ebp
	ret
global read_cr3
read_cr3:
	push ebp
	mov ebp, esp
	mov eax, cr3
	mov esp, ebp
	pop ebp
	ret
global read_eflags
read_eflags:
	push ebp
	mov ebp, esp
	pushf ; Just push the return value and return.
	pop eax
	mov esp, ebp
	pop ebp
	ret
