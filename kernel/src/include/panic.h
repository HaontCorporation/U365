#ifndef PANIC_H
#define PANIC_H

#include <isr.h>

void panic(const char *msg, isr *regs);

#endif // PANIC_H