
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <panic.h>
#include <stdio.h>
#include <idt.h>
#include <isr.h>

extern void *isr0[];
extern void *isr1[];
extern void *isr2[];
extern void *isr3[];
extern void *isr4[];
extern void *isr5[];
extern void *isr6[];
extern void *isr7[];
extern void *isr8[];
extern void *isr9[];
extern void *isr10[];
extern void *isr10[];
extern void *isr11[];
extern void *isr12[];
extern void *isr13[];
extern void *isr14[];
extern void *isr15[];
extern void *isr16[];
extern void *isr17[];
extern void *isr18[];
extern void *isr19[];
extern void *isr20[];
extern void *isr21[];
extern void *isr22[];
extern void *isr23[];
extern void *isr24[];
extern void *isr25[];
extern void *isr26[];
extern void *isr27[];
extern void *isr28[];
extern void *isr29[];
extern void *isr30[];
extern void *isr31[];

void isr_install()
{
    idt_set_gate(0, isr0, 0x08, 0x8E);
    idt_set_gate(1, isr1, 0x08, 0x8E);
    idt_set_gate(2, isr2, 0x08, 0x8E);
    idt_set_gate(3, isr3, 0x08, 0x8E);
    idt_set_gate(4, isr4, 0x08, 0x8E);
    idt_set_gate(5, isr5, 0x08, 0x8E);
    idt_set_gate(6, isr6, 0x08, 0x8E);
    idt_set_gate(7, isr7, 0x08, 0x8E);
    idt_set_gate(8, isr8, 0x08, 0x8E);
    idt_set_gate(9, isr9, 0x08, 0x8E);
    idt_set_gate(10, isr10, 0x08, 0x8E);
    idt_set_gate(11, isr11, 0x08, 0x8E);
    idt_set_gate(12, isr12, 0x08, 0x8E);
    idt_set_gate(13, isr13, 0x08, 0x8E);
    idt_set_gate(14, isr14, 0x08, 0x8E);
    idt_set_gate(15, isr15, 0x08, 0x8E);
    idt_set_gate(16, isr16, 0x08, 0x8E);
    idt_set_gate(17, isr17, 0x08, 0x8E);
    idt_set_gate(18, isr18, 0x08, 0x8E);
    idt_set_gate(19, isr19, 0x08, 0x8E);
    idt_set_gate(20, isr20, 0x08, 0x8E);
    idt_set_gate(21, isr21, 0x08, 0x8E);
    idt_set_gate(22, isr22, 0x08, 0x8E);
    idt_set_gate(23, isr23, 0x08, 0x8E);
    idt_set_gate(24, isr24, 0x08, 0x8E);
    idt_set_gate(25, isr25, 0x08, 0x8E);
    idt_set_gate(26, isr26, 0x08, 0x8E);
    idt_set_gate(27, isr27, 0x08, 0x8E);
    idt_set_gate(28, isr28, 0x08, 0x8E);
    idt_set_gate(29, isr29, 0x08, 0x8E);
    idt_set_gate(30, isr30, 0x08, 0x8E);
    idt_set_gate(31, isr31, 0x08, 0x8E);
}

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
    "",
	"Reserved"
};

void fault_handler(isr *state)
{
	printf("\n\nException: %s (0x%X)", exception_messages[state->interrupt_no], state->error_code);
    panic(exception_messages[state->interrupt_no], state);
}