#include "debug.h"
#include "ish.h"
#include "memory.h"
#include "stdio.h"
#include "tty_old.h"
#include "time.h"
#include "ext2.h"
#include "init.h"
#include "string.h"
#include "compare.h"
#include "modules.h"
#include "elf_hdr.h"

#include "arch/i686/regs.h"
#include "arch/i686/devices.h"

#include "graphics/surface.h"
#include "graphics/vesa_fb.h"

/*
	===================U365 PROJECT===================
			   Welcome to U365 source code!
	U365 is an OS from scratch at 95% (IDT, GDT and
	some other code is copied with copy notice).
	It's developers are Nikita Ivanov (catnikita255,
	he is the BPS founder) and Dmitry Kychanov (k1-801).
	This is main U365 kernel source file. It contains
	kernel_main function, the actual kernel. It gets
	called from _start, kernel binary entry point.
*/

extern surface* cursor_new;

struct mboot_info *kernel_mboot_info; //Other kernel functions will use it.
void kernel_main(int eax_magic, struct mboot_info *mboot) //Arguments are passed by _start.
{
	kernel_mboot_info=mboot;
	// Init the memory heap first, because NOTHING will work without it AT ALL
	parse_memlist(mboot);
	initVBE(mboot); //Getting info from multiboot structure
	initTTY();      //Initializing terminal
	printk("U365 version " QUOT(OSVER) ", built on " __DATE__ " " __TIME__ "."); //yes, first message looks like in Linux.

	//If the EAX magic number passed by GRUB isn't valid, display error and stop boot process.
	if(eax_magic!=0x2BADB002)
	{
		tty_wrstr("\e[94mInvalid Multiboot magic number!\e[0m\n");
		while(1);
	}
	if(mboot->framebuffer_type!=1) //for text mode
	{
		printk("Non-pixel framebuffer type.");
	}
	tty_wrstr("\e[97mWelcome to U365 " QUOT(OSVER) ". We are almost here, please wait...\e[0m\n\n"
			  "    U365 " QUOT(OSVER) ", developed by BPS. Have a nice day :)");
	tty_putchar('\n');

	//Run pre-run tasks and start the shell.
	os_lowlevel_init(mboot); //Call initializing function.
	/*
	void tui_draw_box(uint8_t color, int x, int y, int w, int h);
	void tui_draw_text(uint8_t color, int x, int y, const char* text);
	void tui_bordered_box(uint8_t, int, int, int, int);
	void tui_fill_area(uint8_t, int, int, int, int, uint8_t);
	//tui_bordered_box(0x07, 0, 0, tty_w, tty_h);
	//tui_fill_area(0xB0, 6, 6, 38, 18, 0x07);
	//tui_draw_text(0x07, 10, 10, "Hello, world!");
	//while(1);*/

    /*char* cmdline[] = {"/sbin/init", NULL};
    run_elf_file(1, cmdline);*/

    srand(time(0));
    /*char* argv[] = {NULL};
    char* envp[] = {NULL};
    run_elf_file("/bin/login", argv, envp);*/ //That WIP feature shouldn't be present in release version
    //ext2_info(0);
    ish2_main(0, 0); // Run an interactive shell with no arguments at all
}

