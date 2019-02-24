#ifndef SYSCALL_WRAPPERS
#define SYSCALL_WRAPPERS

#include <stdio.h>

// FIO
int read(int, void*, int);
int write(int, void*, int);
int open(const char*, const char*);
int close(int);
int fstat(const char*, struct stat *);
int lseek(int, off_t, int);

void*       allocate(int);
const char* getenv(const char*);
void        setenv(const char*, const char*);

#endif
