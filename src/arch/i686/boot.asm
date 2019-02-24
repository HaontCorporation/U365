; U365 boot code, loaded by GRUB bootloader.
; Here's some constants needed for GRUB with annotations.
MBALIGN equ 1<<0 ; Align flag
MEMINFO equ 1<<1 ; Memory info flag, so GRUB will give us the memory info in MB structure.
VBE_MODE equ 1<<2 ; VBE mode flag. GRUB will set it for us and provide info about it.
FLAGS equ MBALIGN | MEMINFO | VBE_MODE ; Flags
MAGIC equ 0x1BADB002 ; Multiboot magic number.
CHECKSUM equ -(MAGIC + FLAGS) ; Multiboot checksum
align 4 ; 4-byte aligned code/data goes after that line.
; Put all our settings there.
dd MAGIC ; Magic number, so GRUB will know that we're not going to kill him
dd FLAGS ; Flags that we're set
dd CHECKSUM ; Checksum
dd 0, 0, 0, 0, 0 ; Unneeded now.
dd 0 ; Set graphics mode
dd 1024, 768, 32 ; WxHxD screen resolution
;Here starts the bootstrap stack for our kernel.
section .bootstrap_stack, nobits ; We're in bootstrap_stack section now.

stack_bottom:
resb 65535 ; 64 KiB for the stack.
stack_top: ; Start of the stack.
section .text ; Finally. Start of the code in .text section.
global _start ; We'll use that as a kernel entry point.
; _start function. Kernel entry point, it sets up the stack and calls kernel_main, the actual C kernel.
_start:
	; Disable interrupts, so it'll be safe
	cli
	; Enable FPU.
	finit
	; Set up the stack, or C code won't work.
	mov esp, stack_top
	; __IMPORTANT__
	; GRUB puts Multiboot structure and magic number in EBX
	; and EAX, so we need to push them as arguments to ker-
	; nel_main.
	push ebx ; Multiboot structure
	push eax ; Multiboot magic number
	; Fire up the OS! Running kernel_main()
	extern kernel_main ; kernel_main() is in another file, so tell it to NASM
	call kernel_main ; Call it!
	; If the kernel someway returned (btw it cannot happen because of while(1)), just halt.
	cli ; Disable the interrupts, so no IRQs can happen.
	hlt ; Halt the CPU - OS kernel is stopped.
