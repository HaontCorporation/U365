#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <syscall_codes.h>
#include <syscall_wrappers.h>

long syscall(long number, ...);

#endif
