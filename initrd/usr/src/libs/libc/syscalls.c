//U365 libc: syscalls

#include <stdarg.h>
#include <stdint.h>

long syscall(long number, ...)
{
	uint32_t save;
	long result;
	asm volatile("mov %%esp, %0;": "=g"(save));
	asm volatile("int $0x80;" : "=a"(result) : "a"(number), "b"(&number));
	asm volatile("mov %0, %%esp;" : : "g"(save));
	return result;
}
