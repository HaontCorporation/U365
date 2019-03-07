#include <debug.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <compare.h>
#include <modules.h>
#include <stdbool.h>
#include <surface.h>
#include <tty.h>
#include <vbe.h>

#define OSVER 1.0

extern void *vgafnt_dump[];

void kernel_main()
{
    init_memory();
    init_stdio();
    initTTY();
    
    init_arch();
    init_surface_screen();

    char *buffer = malloc(512);
    printf("fgets(buffer, 512, stdin): ");
    fgets(buffer, 512, stdin);
    

    tty_wrstr("\e[97mWelcome to U365. We are almost here, please wait...\e[0m\n\n"
		  "    U365, developed by Haont. Have a nice day :)");
}