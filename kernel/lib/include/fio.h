#ifndef FIO_H
#define FIO_H

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#ifndef EOF
#define EOF -1
#define SEEK_SET -1
#define SEEK_CUR 0
#define SEEK_END 1
#endif

//typedef struct inode_t inode_t;
//typedef struct __FILE __FILE;
typedef struct __FILE FILE;
typedef unsigned long off_t;
typedef int mode_t;

#include <devtab.h>

#ifdef __cplusplus
extern "C" {
#endif
struct inode_t
{
	uint64_t start;  // start offset on device - shall be 0 for device inodes
	uint64_t size;   // size of file - shall be 0 for device inodes, unuseful for directory inodes

//	rights_t rights; // POSIX access rights and flags s-rwx-rwx-rwx
	int dir     : 1;
	int exists  : 1;
	int symlink : 1;
};

struct __FILE
{
	void*    data; // IU
	dev_t    device;
	uint64_t inode;
	int      fd;
	uint32_t position;
	int     (*seek )(FILE*, off_t, int);
	int16_t (*getc )(FILE*);
	int16_t (*putc )(FILE*, char);
	size_t  (*gets )(FILE*, char*, size_t);
	size_t  (*puts )(FILE*, const char*);
	size_t  (*size )(FILE*);
	void    (*close)(FILE*);
};


struct _IO_FILE
{
  int _flags;		/* High-order word is _IO_MAGIC; rest is flags. */
#define _IO_file_flags _flags

  /* The following pointers correspond to the C++ streambuf protocol. */
  /* Note:  Tk uses the _IO_read_ptr and _IO_read_end fields directly. */
  char* _IO_read_ptr;	/* Current read pointer */
  char* _IO_read_end;	/* End of get area. */
  char* _IO_read_base;	/* Start of putback+get area. */
  char* _IO_write_base;	/* Start of put area. */
  char* _IO_write_ptr;	/* Current put pointer. */
  char* _IO_write_end;	/* End of put area. */
  char* _IO_buf_base;	/* Start of reserve area. */
  char* _IO_buf_end;	/* End of reserve area. */
  /* The following fields are used to support backing up and undo. */
  char *_IO_save_base; /* Pointer to start of non-current get area. */
  char *_IO_backup_base;  /* Pointer to first valid character of backup area */
  char *_IO_save_end; /* Pointer to end of non-current get area. */

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;
};

int open(const char*, int, mode_t);
FILE*   fopen  (const char*, const char*);
void    freopen(const char*, const char*, FILE*);
int16_t fgetc  (FILE*);
int16_t fputc  (char, FILE*);
size_t  fgets  (char*, size_t, FILE*);
size_t  fputs  (const char*, FILE*);
void    fclose (FILE*);
size_t  fread  (void*, size_t, size_t, FILE*);
int     fseek  (FILE*, off_t, int);

int  fprintf(FILE*, const char*, ...);
int vfprintf(FILE*, const char*, va_list);

void perr(const char*);

#ifdef __cplusplus
}
#endif

#endif // FIO_H