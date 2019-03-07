#include <stdio.h>

#include <memory.h>
#include <errno.h>
#include <fs.h>
#include <kbd.h>
#include <string.h>

FILE *stdin  = 0;
FILE *stdout = 0;
FILE *stderr = 0;

void perr(const char* str)
{
	fputs(str, stderr);
}

int16_t stdio_tty_putc(FILE* UNUSED(this), char c)
{
	tty_putchar(c);
	return c;
}

size_t stdio_tty_puts(FILE* UNUSED(this), const char* s)
{
	tty_wrstr(s);
	return strlen(s);
}

int16_t stdio_kbd_getc(FILE* UNUSED(this))
{
	return kbd_getchar();
}

size_t stdio_kbd_gets(FILE* UNUSED(this), char* s, size_t num)
{
	return kbd_gets(s, num);
}

void init_stdio()
{
	stdin  = (FILE *)(malloc(sizeof(FILE)));
	stdout = (FILE *)(malloc(sizeof(FILE)));
    stderr = (FILE *)(malloc(sizeof(FILE)));

	stdout->putc = &stdio_tty_putc;
	stdout->puts = &stdio_tty_puts;
	stderr->putc = &stdio_tty_putc;
	stderr->puts = &stdio_tty_puts;
	stdin->getc = &stdio_kbd_getc;
	stdin->gets = &stdio_kbd_gets;
}

int16_t putchar(char c)
{
	return fputc(c, stdout);
}
size_t puts(const char* str)
{
	return fputs(str, stdout);
}
int16_t getchar()
{
	return fgetc(stdin);
}
size_t stdio_gets(char* str, size_t size)
{
	return fgets(str, size, stdin);
}

int printf(const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int result = vfprintf(stdout, fmt, va);
	va_end(va);
	return result;
}

// int vprintf(const char* fmt, va_list va)
// {
// 	return vfprintf(stdout, fmt, va);
// }

// int sprintf(char* str, const char* fmt, ...)
// {
// 	va_list va;
// 	va_start(va, fmt);
// 	int result = vsprintf(str, fmt, va);
// 	va_end(va);
// 	return result;
// }
// /*
//  * perror - displaying error messages.
//  * It displays mesg, then ": " and cur-
//  * rent error message from errno global
//  * variable.
// */
// void perror(const char* mesg)
// {
// 	//Actually very small function.
// 	printf("%s: %s\n", mesg, strerror(errno));
// }