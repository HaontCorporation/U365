#include <idt.h>

#include <stdint.h>
#include <stddef.h>

#include <stdio.h>
#include <io.h>
#include <panic.h>
#include <debug.h>
#include <memory.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>
//#include <arch/i686/rtc.h>

idt_entry idt[256];
idtr _idtr;

void idt_set_gate(uint8_t num, uint64_t base, uint16_t selector, uint8_t flags)
{
	idt[num].base_lo = (base & 0xFFFF); // the lower 16-bit of the base
	idt[num].base_hi = (base >> 16) & 0xFFFFFFFFFFFF; // the higher 48-bit of the base

	idt[num].selector = selector;
	idt[num].always0 = 0;
	idt[num].flags = flags;
}

void idt_install()
{
	_idtr.limit = (sizeof (struct idt_entry) * 256) - 1;
	_idtr.base = (uint64_t)&idt;
	memset(_idtr.base, 0, _idtr.limit);

	/* Install Interrupt Handlers */
	isr_install();
	irq_install();

	asm volatile ("lidt [_idtr]");
}