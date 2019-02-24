; Kernel heap filling
; DOESN'T WORK, WRECKING UP OS
;section .kheap ; We're in .kheap section
;kernel_heap_start:
	;resb 65536 ; Reserve 64 KB for kernel heap.