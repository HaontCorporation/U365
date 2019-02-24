#ifndef STDIO_H
#define STDIO_H
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#ifndef EOF
#define EOF -1
#define SEEK_SET -1
#define SEEK_CUR 0
#define SEEK_END 1
#define OPEN_MAX 255
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct inode_t inode_t;
typedef struct __FILE __FILE;
typedef struct __FILE FILE;
typedef uint16_t rights_t;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct __FILE
{
  	int      fd;
	uint32_t position;
	uint8_t  exists;
	int     (*seek )(FILE*, long int, int);
	int16_t (*getc )(FILE*);
	int16_t (*putc )(FILE*, char);
	size_t  (*gets )(FILE*, char*, size_t);
	size_t  (*puts )(FILE*, const char*);
	size_t  (*size )(FILE*);
	void    (*close)(FILE*);
};

extern FILE _iob[OPEN_MAX];
#define stdin  (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

FILE*   fopen  (const char*, const char*);
void    freopen(const char*, const char*, FILE*);
int16_t fgetc  (FILE*);
int16_t fputc  (char, FILE*);
size_t  fgets  (char*, size_t, FILE*);
size_t  fputs  (const char*, FILE*);
void    fclose (FILE*);
size_t  fread  (void*, size_t, size_t, FILE*);
int     fseek  (FILE*, long int, int);
int16_t putchar(char);
size_t  puts(const char*);
int16_t getchar();

int  fprintf(FILE*, const char*, ...);
int vfprintf(FILE*, const char*, va_list);

#ifdef __cplusplus
}
#endif
#endif