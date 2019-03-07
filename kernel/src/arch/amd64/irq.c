
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <idt.h>
#include <irq.h>

extern void *irq0[];
extern void *irq1[];
extern void *irq2[];
extern void *irq3[];
extern void *irq4[];
extern void *irq5[];
extern void *irq6[];
extern void *irq7[];
extern void *irq8[];
extern void *irq9[];
extern void *irq10[];
extern void *irq11[];
extern void *irq12[];
extern void *irq13[];
extern void *irq14[];
extern void *irq15[];

irq irq_handlers[16];

int irq_register(int num, void *handler, void *data)
{
    irq *irq = &irq_handlers[num];
    int i; for (i = 0; irq->handler[i]; i++);
    irq->handler[i] = handler;
    irq->data[i] = data;
    irq->present = true;
    irq_unmask(num);
    return i;
}

void irq_unregister(int num, int id)
{
    irq *irq = &irq_handlers[num];
    irq->handler[id] = 0;
    irq->data[id] = 0;

    int i; for (; i < 8; i++)
    {
        if (irq->handler[id]) break;
    }
    if (i == 8) {
        irq->present = false;
        irq_ignore(num);
    }
}

void irq_ignore(int num)
{
	uint16_t port = 0x21;
	uint8_t value;

	if (num >= 8)
	{
		port += 0x80;
		num -= 8;
	}

	value = inb(port) | (1 << num);
	outb(port, value);
}

void irq_unmask(int num)
{
	uint16_t port = 0x21;
	uint8_t value;

	if (num >= 8)
	{
		port += 0x80;
		num -= 8;
	}

	value = inb(port) & ~(1 << num);
	outb(port, value);
}

void irq_remap(int master_offset, int slave_offset)
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, master_offset);
	outb(0xA1, slave_offset);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0xFF);
	outb(0xA1, 0xFF);
}

void irq_install()
{
    irq_remap(0x20, 0x28);
    idt_set_gate(32, irq0, 0x08, 0x8E);
    idt_set_gate(33, irq1, 0x08, 0x8E);
    idt_set_gate(34, irq2, 0x08, 0x8E);
    idt_set_gate(35, irq3, 0x08, 0x8E);
    idt_set_gate(36, irq4, 0x08, 0x8E);
    idt_set_gate(37, irq5, 0x08, 0x8E);
    idt_set_gate(38, irq6, 0x08, 0x8E);
    idt_set_gate(39, irq7, 0x08, 0x8E);
    idt_set_gate(40, irq8, 0x08, 0x8E);
    idt_set_gate(41, irq9, 0x08, 0x8E);
    idt_set_gate(42, irq10, 0x08, 0x8E);
    idt_set_gate(43, irq11, 0x08, 0x8E);
    idt_set_gate(44, irq12, 0x08, 0x8E);
    idt_set_gate(45, irq13, 0x08, 0x8E);
    idt_set_gate(46, irq14, 0x08, 0x8E);
    idt_set_gate(47, irq15, 0x08, 0x8E);
}

void irq_eoi(int num)
{
	outb((num >= 8) ? 0x80 : 0x20, 0x20);
}

void irq_handler(int irq_num)
{
    irq *irq = &irq_handlers[irq_num];
    irq_state (*handler)(void *data);

    if (irq->present)
    {
        for (int i = 0; i < 8; i++)
            if (handler = irq->handler[i])
                if (handler(irq->data[i]) == IRQ_UNHANDLED) // Call each handler that's registered to this IRQ
                    printf("irq_handler(): unhandled irq (handler returned unhadled)\n");
        irq_eoi(irq_num);
    }
    else printf("irq_handler(): unhandled irq (irq is not registered), aborting...\n");
}