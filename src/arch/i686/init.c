#include <stdio.h>

#include <arch/i686/idt.h>
#include <arch/i686/gdt.h>
#include <arch/i686/io.h>

#include <sys.h>
#include <debug.h>
#include <modules.h>
#include <devtab.h>

#include <arch/i686/multitasking.h>
#include <arch/i686/devices.h>
#include <arch/i686/acpi.h>
#include <arch/i686/kbd.h>
#include <arch/i686/asmdefs.h>
#include <arch/i686/smbios.h>
#include <arch/i686/paging.h>

/**
 * This file is part of U365 kernel.
 * DESC
 * Initializing of arch-specific features.
 */

void arch_init() //Initializing of x86-specific features.
{
	outb(0x21,0xfd);
	outb(0xa1,0xff);
	init_pics(0x20,0x28);
	gdt_install();
	idt_install();
	isrs_install();
	irq_install();
	//Multitasking needs PIT, so we will initialize it shortly before it.
	//printk("Initializing multitasking");
	//init_multitasking();
	printk("Setting up PIT");
	setupPIT();
	printk("Setting up PS2 keyboard");
	initKbdInt();
	kbd_scancodes_setup();
	printk("Setting up SMBIOS");
	unsigned char *mem = (unsigned char *) 0xF0000;
	int length, i;
	unsigned char checksum;
	while ((unsigned int) mem < 0x100000)
	{
		if (memcmp(mem,"_SM_",4)==0)
		{
			length = mem[5];
			checksum = 0;
			for(i = 0; i < length; i++)
			{
				checksum += mem[i];
			}
			if(checksum == 0) break;
		}
		mem += 16;
	}
	//serial_out("sm\n");
	if ((unsigned int) mem == 0x100000)
	{
		printk("error: SMBIOS not found!");
		//serial_out("smerr\n");
	}
	smb_ep = (struct SMBIOSEntryPoint*)mem;
	printk("SMBIOS found at 0x%08x.");
	smbios_detected=true;
	printk("Getting hardware info from SMBIOS");
	machine_bios_vendor  = detectBIOSVendor();
	machine_bios_version = detectBIOSVersion();
	machine_bios_bdate   = detectBIOSBDate();
	init_acpi();
	//outb(0x21,0xfc);
	//outb(0xa1,0x00);
	//setup_paging();
	for(int i = 0; i < 16; ++i)
	{
		IRQ_clear_mask(i);
	}

	INT_ON;
}
