global load_pg_dir
load_pg_dir:
	push ebp
	mov ebp, esp
	mov eax, [esp+8]
	mov cr3, eax
	mov esp, ebp
	pop ebp
	ret
global paging_enable
paging_enable:
	push ebp
	mov ebp, esp
	mov eax, cr0
	or eax, 0x80000000 ; Enable the paging bit
	mov cr0, eax       ; Fire up the paging!!!
	mov esp, ebp
	pop ebp
	ret