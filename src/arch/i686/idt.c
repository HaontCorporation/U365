#include <arch/i686/idt.h>

#include <stdint.h>
#include <stddef.h>

#include <stdio.h>
#include <arch/i686/io.h>
#include <panic.h>
#include <debug.h>
#include <memory.h>
#include <arch/i686/rtc.h>

// Code from Bran's tutorial. Do not scream that I stealed it!
regs *panicr;
idt_entry idt[256];
idt_ptr   idtp;

void IRQ_set_mask(unsigned char IRQline)
{
	uint16_t port;
	uint8_t value;

	if(IRQline < 8)
	{
		port = PIC1_DATA;
	}
	else
	{
		port = PIC2_DATA;
		IRQline -= 8;
	}
	value = inb(port) | (1 << IRQline);
	outb(port, value);
}

void IRQ_clear_mask(unsigned char IRQline)
{
	uint16_t port;
	uint8_t value;

	if(IRQline < 8)
	{
		port = PIC1_DATA;
	}
	else
	{
		port = PIC2_DATA;
		IRQline -= 8;
	}
	value = inb(port) & ~(1 << IRQline);
	outb(port, value);
}

/* Use this function to set an entry in the IDT. Alot simpler
*  than twiddling with the GDT ;) */
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags)
{
	/* The interrupt routine's base address */
	idt[num].base_lo = (base & 0xFFFF);
	idt[num].base_hi = (base >> 16) & 0xFFFF;

	/* The segment or 'selector' that this IDT entry will use
	*  is set here, along with any access flags */
	idt[num].sel = sel;
	idt[num].always0 = 0;
	idt[num].flags = flags;
}


void isrs_install()
{
	idt_set_gate(   0, (unsigned)isr0,  0x08, 0x8E);
	idt_set_gate(   1, (unsigned)isr1,  0x08, 0x8E);
	idt_set_gate(   2, (unsigned)isr2,  0x08, 0x8E);
	idt_set_gate(   3, (unsigned)isr3,  0x08, 0x8E);
	idt_set_gate(   4, (unsigned)isr4,  0x08, 0x8E);
	idt_set_gate(   5, (unsigned)isr5,  0x08, 0x8E);
	idt_set_gate(   6, (unsigned)isr6,  0x08, 0x8E);
	idt_set_gate(   7, (unsigned)isr7,  0x08, 0x8E);
	idt_set_gate(   8, (unsigned)isr8,  0x08, 0x8E);
	idt_set_gate(   9, (unsigned)isr9,  0x08, 0x8E);
	idt_set_gate(  10, (unsigned)isr10, 0x08, 0x8E);
	idt_set_gate(  11, (unsigned)isr11, 0x08, 0x8E);
	idt_set_gate(  12, (unsigned)isr12, 0x08, 0x8E);
	idt_set_gate(  13, (unsigned)isr13, 0x08, 0x8E);
	idt_set_gate(  14, (unsigned)isr14, 0x08, 0x8E);
	idt_set_gate(  15, (unsigned)isr15, 0x08, 0x8E);
	idt_set_gate(  16, (unsigned)isr16, 0x08, 0x8E);
	idt_set_gate(  17, (unsigned)isr17, 0x08, 0x8E);
	idt_set_gate(  18, (unsigned)isr18, 0x08, 0x8E);
	idt_set_gate(  19, (unsigned)isr19, 0x08, 0x8E);
	idt_set_gate(  20, (unsigned)isr20, 0x08, 0x8E);
	idt_set_gate(  21, (unsigned)isr21, 0x08, 0x8E);
	idt_set_gate(  22, (unsigned)isr22, 0x08, 0x8E);
	idt_set_gate(  23, (unsigned)isr23, 0x08, 0x8E);
	idt_set_gate(  24, (unsigned)isr24, 0x08, 0x8E);
	idt_set_gate(  25, (unsigned)isr25, 0x08, 0x8E);
	idt_set_gate(  26, (unsigned)isr26, 0x08, 0x8E);
	idt_set_gate(  27, (unsigned)isr27, 0x08, 0x8E);
	idt_set_gate(  28, (unsigned)isr28, 0x08, 0x8E);
	idt_set_gate(  29, (unsigned)isr29, 0x08, 0x8E);
	idt_set_gate(  30, (unsigned)isr30, 0x08, 0x8E);
	idt_set_gate(  31, (unsigned)isr31, 0x08, 0x8E);
	idt_set_gate(0x80, (unsigned)syscall_handler, 0x08, 0x8E);
}

/* This array is actually an array of function pointers. We use
*  this to handle custom IRQ handlers for a given IRQ */
void *irq_routines[16] =
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

const char *exception_messages[] =
{
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault!",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault!",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
	"Alignment Check Exception (486+)",
	"Machine Check Exception (Pentium/586+)",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};
uint32_t cr0_reg;
void fault_handler(struct regs *r)
{
	//debug_print(ERROR, "CPU Exception Catched!\n");
	tty_wrstr("\n\nException: ");
	tty_wrstr(exception_messages[r->int_no]);
	if(r->int_no==18)
	{
		printk("Machine Check Exception!");
	}
	panicr=r;
	panic(exception_messages[r->int_no]);
}

/* This installs a custom IRQ handler for the given IRQ */
void irq_install_handler(int irq, void (*handler)(struct regs *r))
{
	irq_routines[irq] = handler;
}

/* This clears the handler for a given IRQ */
void irq_uninstall_handler(int irq)
{
	irq_routines[irq] = 0;
}

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
void irq_remap(void)
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}

/* We first remap the interrupt controllers, and then we install
*  the appropriate ISRs to the correct entries in the IDT. This
*  is just like installing the exception handlers */
void irq_install()
{
	irq_remap();

	idt_set_gate(32, (unsigned)irq0,  0x08, 0x8E);
	idt_set_gate(33, (unsigned)irq1,  0x08, 0x8E);
	idt_set_gate(34, (unsigned)irq2,  0x08, 0x8E);
	idt_set_gate(35, (unsigned)irq3,  0x08, 0x8E);
	idt_set_gate(36, (unsigned)irq4,  0x08, 0x8E);
	idt_set_gate(37, (unsigned)irq5,  0x08, 0x8E);
	idt_set_gate(38, (unsigned)irq6,  0x08, 0x8E);
	idt_set_gate(39, (unsigned)irq7,  0x08, 0x8E);
	idt_set_gate(40, (unsigned)irq8,  0x08, 0x8E);
	idt_set_gate(41, (unsigned)irq9,  0x08, 0x8E);
	idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
	idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
	idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
	idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
	idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
	idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
}

/* Each of the IRQ ISRs point to this function, rather than
*  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
*  to be told when you are done servicing them, so you need
*  to send them an "End of Interrupt" command (0x20). There
*  are two 8259 chips: The first exists at 0x20, the second
*  exists at 0xA0. If the second controller (an IRQ from 8 to
*  15) gets an interrupt, you need to acknowledge the
*  interrupt at BOTH controllers, otherwise, you only send
*  an EOI command to the first controller. If you don't send
*  an EOI, you won't raise any more IRQs */
void irq_handler(struct regs *r)
{
	/* This is a blank function pointer */
	void (*handler)(struct regs *r);

	/* Find out if we have a custom handler to run for this
	*  IRQ, and then finally, run it */
	handler = irq_routines[r->int_no - 32];
	if (handler)
	{
		handler(r);
	}

	/* If the IDT entry that was invoked was greater than 40
	*  (meaning IRQ8 - 15), then we need to send an EOI to
	*  the slave controller */
	if (r->int_no >= 40)
	{
		outb(0xA0, 0x20);
	}

	/* In either case, we need to send an EOI to the master
	*  interrupt controller too */
	outb(0x20, 0x20);
	//IMPORTANT: Reenable interrupts.
	//Otherwise the entire OS won't work.
	asm("sti");
}

/* Installs the IDT */
void idt_install()
{
	/* Sets the special IDT pointer up, just like in 'gdt.c' */
	idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
	idtp.base = (uint32_t)&idt;
	/* Clear out the entire IDT, initializing it to zeros */
	memset(&idt, 0, sizeof(struct idt_entry) * 256);
	//isrs_install();

	/* Add any new ISRs to the IDT here using idt_set_gate */



	/* Points the processor's internal register to the new IDT */
	idt_load();
}
