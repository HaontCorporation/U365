#include <stdio.h>
#include <stdint.h>
#include <fio.h>
#include <syscall_wrappers.h>
FILE _iob[OPEN_MAX] = {};

// STDIO FILE* wrappers
int putchar(int c)
{
	return fputc(c, stdout);
}
int puts(const char* s)
{
	return fputs(s, stdout) + !fputc('\n', stdout);
}
int getchar()
{
	return fgetc(stdin);
}

int printf(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int result = vfprintf(stdout, fmt, va);
	va_end(va);
	return result;
}

void init_stdio()
{
	fill_file_struct(stdin,  0);
	fill_file_struct(stdout, 1);
	fill_file_struct(stderr, 2);
}

