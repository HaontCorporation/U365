#ifndef INIT_H
#define INIT_H

#include <arch/i686/mboot.h>

void arch_init();
void os_lowlevel_init(struct mboot_info*);

#endif
