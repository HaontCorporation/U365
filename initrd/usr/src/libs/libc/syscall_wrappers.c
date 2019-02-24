#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <syscall_codes.h>

long syscall(long, ...);

int read(int fd, void* buf, int count)
{
	return syscall(SYS_read, fd, buf, count);
}
int write(int fd, void* buf, int count)
{
	return syscall(SYS_write, fd, buf, count);
}
int open(const char* name, const char* mode)
{
	return syscall(SYS_open, name, mode);
}
int close(int fd)
{
	return syscall(SYS_close, fd);
}
int lseek(int fd, off_t offset, int origin)
{
	return syscall(SYS_lseek, fd, offset, origin);
}
const char* getenv(const char* key)
{
	return (const char*) syscall(SYS_T_env_get, key);
}
void setenv(const char* key, const char* value)
{
	syscall(SYS_T_env_set, key, value);
}
void *allocate(int num)
{
	return (void*)syscall(SYS_T_alloc, num);
}
int fstat(const char* name, struct stat *st)
{
	return syscall(SYS_stat, name, st);
}
