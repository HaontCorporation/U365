#include <syscall_wrappers.h>
#include <stddef.h>
#include <stdint.h>
#include <memory.h>

int memcmp(const void *s1, const void *s2, size_t n)                    /* Length to compare. */
{
	unsigned char u1, u2;

	for ( ; n-- ; s1++, s2++) {
	u1 = * (unsigned char *) s1;
	u2 = * (unsigned char *) s2;
	if ( u1 != u2) {
		return (u1-u2);
	}
	}
	return 0;
}
void* memcpy(void* dst, const void* src, size_t n)
{
	size_t i;
	if((uint8_t*)(dst) < (uint8_t*)(src))
	{
		for(i = 0; i < n; ++i)
		{
			((uint8_t*)(dst))[i] = ((uint8_t*)(src))[i];
		}
	}
	else
	{
		for(i = 0; i < n; ++i)
		{
			((uint8_t*)(dst))[n - i - 1] = ((uint8_t*)(src))[n - i - 1];
		}
	}
	return dst;
}

void* memset(void* ptr, uint8_t val, size_t n)
{
	size_t i;
	for(i = 0; i < n; ++i)
	{
		((uint8_t*)(ptr))[i] = val;
	}
	return ptr;
}

void* memmove(void* dst, void* src, size_t n)
{
	if(dst != src)
	{
		memcpy(dst, src, n);
	}
	return dst;
}
void *malloc(int num)
{
	return allocate(num);
}
void* calloc(int num, int block_num)
{
	void* ptr = malloc(num*block_num);
	//memset(ptr, 0, num*block_num);
	return ptr;
}
void* realloc(void* src, int size)
{
    void* ptr = malloc(size);
    memcpy(src, ptr, size);
    free(src);
    return ptr;
}
void free(void* ptr __attribute__((unused)))
{
	return;
}
