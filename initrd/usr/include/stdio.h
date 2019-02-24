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
typedef unsigned long off_t;

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
};
typedef uint32_t ino_t;
typedef uint32_t dev_t;

typedef int mode_t;
struct stat
{
	dev_t     st_dev;         /* ID of device containing file */
	ino_t     st_ino;         /* inode number */
	mode_t    st_mode;        /* file type and mode */
/*	nlink_t*/   int st_nlink;       /* number of hard links */
//	uid_t     st_uid;         /* user ID of owner */
//	gid_t     st_gid;         /* group ID of owner */
	dev_t     st_rdev;        /* device ID (if special file) */
/*	off_t*/     int st_size;        /* total size, in bytes */
//	blksize_t st_blksize;     /* blocksize for filesystem I/O */
//	blkcnt_t  st_blocks;      /* number of 512B blocks allocated */
};


extern FILE _iob[OPEN_MAX];
#define stdin  (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

// Open
FILE* fopen  (const char*, const char*);
void  freopen(const char*, const char*, FILE*);
void  fclose (FILE*);
int   stat   (const char*, struct stat*);
// Low
size_t  fread  (void*, size_t, size_t, FILE*);
int     fseek  (FILE*, off_t, int);
// Get
int   fgetc  (FILE*);
char* fgets  (char*, int, FILE*);
int   getc   (FILE*);
int   getchar();
char* gets   (char*); // DEPRECATED, Might be not implemented
// Put
int fputc  (int,         FILE*);
int fputs  (const char*, FILE*);
int putc   (int,         FILE*);
int putchar(int);
int puts   (const char*);

// Advanced
int  fprintf(FILE*, const char*, ...);
int vfprintf(FILE*, const char*, va_list);
int   printf(const char*, ...);

#ifdef __cplusplus
}
#endif
#endif
