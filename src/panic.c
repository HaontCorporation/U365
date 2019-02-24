#include <panic.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arch/i686/asmdefs.h>
#include <arch/i686/idt.h>

void panic(const char *mesg)
{
	INT_OFF;
	uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, cr0, cr2, cr3;
	asm("mov %%eax, %0;\
		 mov %%ebx, %1;\
		 mov %%ecx, %2;\
		 mov %%edx, %3;\
		 mov %%esi, %4;\
		 mov %%edi, %5;\
		 mov %%esp, %6;\
		 mov %%ebp, %7;\
		 mov %%cr0, %8;\
		 mov %%cr2, %9;\
		 mov %%cr3, %10;\
		 " : "=g" (eax),
			 "=g" (ebx),
			 "=g" (ecx),
			 "=g" (edx),
			 "=g" (esi),
			 "=g" (edi),
			 "=g" (esp),
			 "=g" (ebp),
			 "=g" (cr0),
			 "=g" (cr2),
			 "=g" (cr3)
			);
	printf("\npanic(): %s\n\n",mesg);
	printf("Register state before a panic:\n");
	printf("EAX=0x%08x  "
		   "EBX=0x%08x  "
		   "ECX=0x%08x  "
		   "EDX=0x%08x  "
		   "EDI=0x%08x  "
		   "ESI=0x%08x  "
		   "EBP=0x%08x\n"
		   "CR0=0b%b\nPage fault linear address (if page fault occured): 0x%08x\n"
		   "Page directory address (if paging is enabled): 0x%08x\n", eax, ebx, ecx, edx, edi, esi, ebp, cr0, cr2, cr3);
	printf("Halting.");
	HALT;
}
