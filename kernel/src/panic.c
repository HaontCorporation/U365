#include <panic.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <asmdefs.h>
#include <isr.h>

void panic(const char *msg, isr *regs)
{
    INT_OFF;
    tty_wrstr("\e[91m");
	printf("\npanic(): %s\n\n", msg);
	printf("Register state before a panic:\n");
	printf("RAX=0x%016llx  "
		   "RBX=0x%016llx  "
		   "RCX=0x%016llx  "
		   "RDX=0x%016llx  "
		   "RDI=0x%016llx\n"
		   "RSI=0x%016llx  "
		   "RBP=0x%016llx  "
           "R08=0x%016llx  "
           "R09=0x%016llx  "
           "R10=0x%016llx\n"
           "R11=0x%016llx  "
           "R12=0x%016llx  "
           "R13=0x%016llx  "
           "R14=0x%016llx  "
           "R15=0x%016llx\n"
		   "CR0=0x%016llx  "
           "CR2=0x%016llx  "
           "CR3=0x%016llx\n"
           "RSP=0x%016llx  "
           "RIP=0x%016llx  "
           "RFL=0x%016llx\n"
           "CS=0x%x  "
           "FS=0x%x  "
           "GS=0x%x  "
           "SS=0x%x\n", regs->rax, regs->rbx, regs->rcx, regs->rdx, regs->rdi, regs->rsi, regs->rbp, regs->r8, regs->r9, regs->r10, regs->r11, regs->r12, regs->r13, regs->r14, regs->r15, regs->cr0, regs->cr2, regs->cr3, regs->rip, regs->rsp, regs->rflags, regs->cs, regs->fs, regs->gs, regs->ss);
	printf("Halting.");
	HALT;
}