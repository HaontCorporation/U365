#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <stddef.h>

#include <tty_old.h>
#include <arch/i686/kbd.h>
#include <fio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

int   printf(       const char *fmt, ...);
int  vprintf(       const char *fmt, va_list);
int  sprintf(char*, const char *fmt, ...);
int vsprintf(char*, const char *fmt, va_list);
int16_t putchar(char);
size_t  puts(const char*);
int16_t getchar();
size_t  stdio_gets(char*, size_t);
void    perror(const char *);

#ifdef __cplusplus
}
#endif

#endif // STDIO_H
