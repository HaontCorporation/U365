
global main

[bits 16]
main:
    mov [DriveIndex], dl
    mov si, message
    call puts

    call load_kernelfile

    cli
    
    lgdt [gdtr]
    call a20_enable

    mov eax, cr0
    or al, 1
    mov cr0, eax
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov eax, cr0
    and al, 0xFE
    mov cr0, eax

	xor ax, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
    
    cli

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp 0x18:pmode32

    cli
    hlt

load_kernelfile:
.load_kernelinfo:
    .loop:
        cld
        mov di, [RootDirBase]
        mov si, KernelName
        mov cx, 11
        repe cmpsb
        je .found
        add byte [RootDirBase], 0x20
        jmp .loop
    .found:
        mov di, [RootDirBase]

        xor dx, dx
        mov ax, word [di+0x1A]
        add ax, 37 ; (1 + 24 + 14 - 2)
        mov word [KernelLBA], ax

        mov eax, dword [di+0x1C]
        mov ebx, 512
        div ebx
        inc eax 
        mov [KernelSectorsCount], ax

.load_kernel:
    mov ax, [KernelSectorsCount]
    mov [dap.SectorsCount], ax

    mov bp, 0x1000
    mov [dap.BufferSegment], bp

    mov ax, [KernelLBA]
    mov word [dap.LBA], ax

    cmp word [dap.SectorsCount], 127
    jg .multiple

    mov ah, 42h
    mov dl, [DriveIndex]
    mov si, dap
    int 0x13
    jc error
        
    ret

    .multiple:
		mov word [dap.SectorsCount], 128
		
		mov ah, 42h
		mov dl, [DriveIndex]
		mov si, dap
		
		int 0x13
        jc error
		
		sub word [KernelSectorsCount], 128
		add word [dap.LBA], 128
		add word [dap.BufferSegment], 1000h
		
		cmp word [KernelSectorsCount], 127
		jg .multiple
		
	.last:
		mov ax, [KernelSectorsCount]
		mov [dap.SectorsCount], ax
		
		mov ah, 42h
		mov dl, [DriveIndex]
		mov si, dap
		
		int 0x13
        jc error
		
		ret

a20_check:
	xor ax, ax
	mov es, ax
	not ax
	mov fs, ax
	
	mov ax, word [es:0x7FDE]
	cmp word [fs:0x7E0E], ax
	je .change_values
	
	.enabled:
		clc
		jmp .done
		
	.change_values:
	
	mov word [es:0x7FDE], 0x1234
	cmp word [fs:0x7E0E], 0x1234
	jne .enabled
	
	stc
	
	.done:
		mov word [es:0x7FDE], ax
		ret
		
a20_enable:
	call a20_check
	jnc .done

	.bios:
		mov ax, 0x2401
		int 0x15
	
	call a20_check
	jnc .done
	
	.keyboard:
		cli
		
		call .a20wait
		mov al, 0xAD
		out 0x64, al
		
		call .a20wait
		mov al, 0xD0
		out 0x64, al
		
		call .a20wait2
		in al, 0x60
		push eax
		
		call .a20wait
		mov al, 0xD1
		out 0x64, al
		
		call .a20wait
		pop eax
		or al, 2
		out 0x60, al
		
		call .a20wait
		mov al, 0xAE
		out 0x64, al
		
		call .a20wait
		sti
		
		call a20_check
		
		.a20wait:
			in al, 0x64
			test al, 2
			jz .a20wait
			ret
		
		.a20wait2:
			in al, 0x64
			test al, 1
			jz .a20wait2
			ret
			
	call a20_check
	
	.done:
		ret

error:
    mov di, errormsg
    call puts

    xor ax, ax
    int 0x16

    db 0xEA
    dd 0xFFFF0000
errormsg: db 'Error occured while loading the kernel, press any key to reboot', 0Dh, 0Ah, 0

puts:
.loop:
    mov ah, 0Eh
    mov al, byte [si]
    xor bx, bx
    int 0x10
    inc si
    cmp byte [si], 0
    jnz .loop
    
    ret

bits 32
extern load_elf
extern entry

pmode32:
    mov ax, 0x20
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    push 0x10000
    call load_elf
    add esp, 4

    call setup_paging
    mov cr3, eax

    ; long mode stuff
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    jmp 0x28:lmode64

section .bss

align 4096
paging_pml4:
    resq 512
paging_pdpt_higherhalf:
    resq 512
paging_pdpt_lowerhalf:
    resq 512

section .text

setup_paging:
    cld
    mov eax, paging_pdpt_lowerhalf
    or eax, 0x03
    mov edi, paging_pml4
    stosd
    xor eax, eax
    stosd

    mov eax, paging_pdpt_higherhalf
    or eax, 0x03
    mov edi, paging_pml4 + (511*8)
    stosd
    xor eax, eax
    stosd

    mov eax, 0x83
    mov edi, paging_pdpt_lowerhalf
    stosd
    xor eax, eax
    stosd

    ; Map for Memory-mapped IO Registers
    mov eax, 0xC0000083
    mov edi, paging_pdpt_lowerhalf + (3 * 8)
    stosd
    xor eax, eax
    stosd

    mov eax, 0x83
    mov edi, paging_pdpt_higherhalf + (511 * 8)
    stosd
    xor eax, eax
    stosd

    mov eax, paging_pml4

    ret

[bits 64]
lmode64:
    mov ax, 0x30
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

	mov rax, [entry]
    jmp rax

[bits 32]

message: db 'Bootloader: Stage2', 0Dh, 0Ah, 0
DriveIndex: db 0
BytesPerSector: dw 512
RootDirEntries: dw 224

RootDirBase: dw 0x500
KernelName: db 'KERNEL  ELF'
KernelSectorsCount: dw 0
KernelLBA: dq 0

gdt:
    .Null:
        dq 0

    .CodeSegment16:
	    dw 0FFFFh
	    dw 0
	    db 0
	    db 10011010b
	    db 10001111b
	    db 0

    .DataSegment16:
	    dw 0FFFFh
	    dw 0
	    db 0
	    db 10010010b
	    db 10001111b
	    db 0

    .CodeSegment32:
        dw 0FFFFh
        dw 0
        db 0
        db 10011010b
        db 11001111b
        db 0

    .DataSegment32:
        dw 0FFFFh
        dw 0
        db 0
        db 10010010b
        db 11001111b
        db 0

    .CodeSegment64:
        dd 0
        db 0
        db 10011010b
        db 00100000b
        db 0

    .DataSegment64:
        dd 0
        db 0
        db 10010010b
        db 00000000b
        db 0
gdtr:
    dw gdtr - gdt - 1
    dq gdt

dap: ; Disk Address Packet
.DAPSize:       dw 0x10 ; Always 0x10
.SectorsCount:  dw 0 ; (some Phoenix BIOSes are limited to a maximum of 127 sectors)
.BufferOffset:  dw 0
.BufferSegment: dw 0
.LBA:           dq 0