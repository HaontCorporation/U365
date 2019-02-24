#include <init.h>

#include <stddef.h>
#include <debug.h>
#include <panic.h>
#include <devtab.h>
#include <proctab.h>
#include <modules.h>

#ifdef MODULE_i686
#include <arch/i686/devices.h>
#include <arch/i686/io.h>
#include <arch/i686/mboot.h>
#include <arch/i686/smbios.h>
#include <arch/i686/ata.h>
#endif // MODULE_i686

#include <tty_old.h>
#include <stdio.h>
#include <vfs/tarfs.h>
#include <compare.h>
#include <display.h>
#include <graphics/surface.h>
#include <graphics/vesa_fb.h>
#include <time.h>

// Initializing functions are only used in there, so we shouldn't declare them in their headers
void   arch_init();
void   init_stdio();
void   setup_env();
void   setupPIT();
void   mouse_install();
bool __u365_init_done=false;

static char* motd_messages[] = {
	"Loading finished. Neurotoxine compressor activated, five minutes to death.",
    "In Soviet Russia, U365 loads YOU!",
	"We will crash soon. Hurry, try to do at least something! You lost.",
	"Join us now and share the crashes, you'll be free, hackers, you'll be free...",
	"General Failure reading disk A:",
	"Starting Windows 95...",
	"U365 is the best OS. It can't corrupt your data, guess why? Because it can't read it!",
    "Artem Pityurenko, kill yourself! I advise Vitaly Kulagin to do the same.",
//	"DIAMONDSOFT SUX",
//	"ARTEM KLIMOV U CANT CODE LLALALALALA",
	"Thanks to all OSDev guys, they helped a lot!",
	"It's over, isn't it? Isn't it? Isn't it over?",
    "youtube.com/osdever ;v",
	"Guess what? BUTT: (_I_)",
	"America didn't grow up listening to us. It still doesn't.",
//	" - Son, we are in U365. Don't write testelf, please.\n - Daddy, why? Let me try, just once...\n - NOOO!\n   Nobody survived after so many errors... RIP.",
};

struct loading_routine {
	const char* desc;
	void (*fcn)();
};

static struct loading_routine routines[] = {
	{"Architecture-specific features initialization", arch_init},
	{"Surface initialization", init_surface_screen},
  //  {"Display initializion",   displays_init},
	{"Setting up envinroment variables", setup_env},
	{"Getting mtab ready", mtab_init}
#ifdef MODULE_MOUSE
	,{"Setting up PS/2 mouse driver", mouse_install}
#endif
#ifdef MODULE_DEVTAB
	,{"Setting up /dev", devtab_init}
#endif
#ifdef MODULE_TARFS
	,{"Setting up TarFS", init_rootfs}
#endif
#ifdef MODULE_PROCTAB
	,{"Setting up /proc", proctab_init}
#endif
#ifdef MODULE_ATA
	,{"Setting up ATA driver", ata_init}
#endif
};
void os_lowlevel_init(struct mboot_info *mboot)
{
	init_stdio(); //Initializing standard I/O streams, so we can use our printf, putchar and more.
	if((mboot->mem_upper+mboot->mem_lower)/1024+1<16) // U365 on a system with less than 16 MB of RAM may misbehave
	{
		//Print the message and halt.
		printf("Non-compatible PC. Minimal RAM amount for U365 is 16 MB, but you have only %d MB.\n", (mboot->mem_upper+mboot->mem_lower)/1024+1);
		panic("Non-compatible PC.");
	}
	for(unsigned i=0; i<sizeof(routines)/sizeof(struct loading_routine); i++)
	{
		printk("[Routine %i]: %s", i, routines[i].desc);
		routines[i].fcn();
		/*sleep(100);*/
	}

	//Setting the __u365_init_done variable, so some functions can know that we're not in boot process.
	__u365_init_done = true;
	//ide_initialize(0x1F0, 0x3F4, 0x170, 0x374, 0x000);

		initTTY();
	printk("U365 initialization finished, giving control to shell.");
	char* motd;
	motd = motd_messages[time(NULL) % (sizeof(motd_messages) / sizeof(char*))];
	tty_wrstr("\e[97m");
	printf("%s", motd);
	tty_wrstr("\e[0m\n\n");
	tty_wrstr("\tU     U\n"
				"\tU  3  U\n"
				"\tU  6  U\n"
				"\tU  5  U\n"
				"\tU     U\n"
				"\t UUUUU \n\n"
				"\e[97mType 'help' for more information\e[0m\n\n"); //Displaying colourful welcome message to the screen to indicate end of boot process.
	return;
}

