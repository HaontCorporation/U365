//Memory management.
#include "memory.h"

#include <stddef.h>
#include <stdbool.h>
#include <panic.h>
#include <string.h>
#include <stdint.h>
#include <fio.h>
#include <stdio.h>

#define isBitSet(var, n) (!!(var & (1 << n)))
void* calloc(int num, int size)
{
	size *= num;
	void* result = malloc(size);
	if(result)
		memset(result, 0, size);
	return result;
}

void* realloc(void* memblock, int size)
{
	if(memblock < (void*)0x500)
	{
		return malloc(size);
	}
	else
	{
		void* newmem = malloc(size);
		if(newmem)
		{
			memmove(newmem, memblock, size);
			free(memblock);
		}
		return newmem;
	}
}

void free(void* ptr __attribute__((unused)))
{
	return;
}
