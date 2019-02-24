#include <libc/fio.h>
#include <arch/i686/io.h>
#include <memory.h>
#include <libc/stdio.h>
#include <debug.h>

typedef struct portdata
{
	uint16_t port;
} portdata;

typedef struct ps2data
{
	// Independent bits showing if device on a port is present;
	int present1 : 1;
	int present2 : 1;
	// 0: neither port is used; 1: first port is used; 2: second port is used;
	int lock : 2;
	// What the...
	int BULBULATORSTYLE : 4;
	// May be will not even be used
	uint8_t devtype1;
	uint8_t devtype2;
	// May be we should add some kind of queues/buffers?
} ps2data;

// getc functions

uint8_t ps2_wait(uint8_t bit, uint8_t val)
{
	for(int i = 0; i < 10000; ++i)
	{
		if(!!(inb(0x64) & (1 << bit)) == val)
		{
			return 1;
		}
	}
	return 0;
}

void ps2_flush()
{
	while(inb(0x64) & 1) inb(0x60);;
}

// Note that it does NOT know what is the source - controller or one of the devices
int16_t ps2_getc(FILE* UNUSED(file))
{
	if(ps2_wait(0, 0))
		return inb(0x60);
	else
		return EOF;
}

int16_t ps2_port1_putc(FILE* UNUSED(file), char data)
{
	if(ps2_wait(1, 0))
	{
		outb(0x60, data);
		return data;
	}
	else
		return EOF;
}

int16_t ps2_port2_putc(FILE* file, char data)
{
	outb(0x64, 0xD4);
	return ps2_port1_putc(file, data);
}

// This should open BOTH PS/2 port files
// Probably platform-dependent
// This function should only be called if the PS/2 controller 100% exists (check FADT first)
void init_ps2()
{
	// First, init the controller
	// Now, init the port files
	ps2data* data = calloc(sizeof(ps2data), 1);
	FILE* f1;
	FILE* f2;
	uint8_t dc = 0;
	// Maybe additional test if controller is present
	{
		// Init the controller here
		outb(0x64, 0xAD);
		outb(0x64, 0xA7);
		ps2_flush();
		// Read configuration
		outb(0x64, 0x20);
		uint8_t conf = ps2_getc(0);
		// clear bits 0, 1 and 6
		conf &= 0b10111100;
		conf &= 0xbc;
		// Write configuration
		outb(0x64, 0x60);
		ps2_port1_putc(0, conf);
		// Run self-test
		outb(0x64, 0xAA);
		if(ps2_getc(0) != 0x5C)
		{
			printk("PS/2 controller error\n");
			return;
		}
		if(conf & 0x10)
		{
			outb(0x64, 0xA8);
			outb(0x64, 0x20);
			conf = ps2_getc(0);
			if(!(conf & 0x10))
				dc = 1;
			outb(0x64, 0xA7);
		}

		// Now, init the files
		data->present1 = 1;
		f1 = calloc(sizeof(FILE), 1);
		f1->data = (void*)(data);
		f1->getc = &ps2_getc;
		f1->putc = &ps2_port1_putc;
		// if double-channel
		if(dc)
		{
			data->present2 = 1;
			f2 = calloc(sizeof(FILE), 1);
			f2->data = (void*)(data);
			f2->getc = &ps2_getc;
			f2->putc = &ps2_port2_putc;
		}
	}
}

