#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

int16_t stdio_getc(FILE*);
int16_t stdio_putc(FILE*, char);
size_t stdio_puts(FILE*, const char *);
void fill_file_struct(FILE*, int);