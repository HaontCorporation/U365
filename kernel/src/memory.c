//Memory management.
#include <memory.h>

#include <stddef.h>
#include <stdbool.h>
#include <panic.h>
#include <string.h>
#include <stdint.h>
#include <fio.h>
#include <stdio.h>

block blocks[1024];
uint8_t *heap_ptr;

#define isBitSet(var, n) (!!(var & (1 << n)))
void* calloc(int num, int size)
{
	size *= num;
	void *result = malloc(size);
	if (result)
		memset(result, 0, size);
	return result;
}

void *realloc(void *memblock, int size)
{
	if(memblock < (void*)0x500)
	{
		return malloc(size);
	}
	else
	{
		void *newmem = malloc(size);
		if(newmem)
		{
			memmove(newmem, memblock, size);
			free(memblock);
		}
		return newmem;
	}
}

void *add_block(void *addr, int size, int used)
{
    if (!size) return NULL;

    int i = 0; for (i = 0; blocks[i].present; i++) ;
    blocks[i].present = true;
    blocks[i].addr = addr;
    blocks[i].size = size;
    blocks[i].used = used;
    return addr;
}

void *malloc(int size)
{
    if (blocks[1023].present) {
        printf("error: heap blocks overflow\n");
        return NULL;
    }

    for (int i = 0; blocks[i].present; i++)
    {
        if (blocks[i].used && blocks[i].size >= size)
        {
            add_block(blocks[i].addr + size, blocks[i].size - size, true);
            blocks[i].size = size; // Resize the block
            blocks[i].used = false;
            return blocks[i].addr;            
        }
    }

    void *addr = add_block(heap_ptr, size, false);
    heap_ptr += size;
    return addr;
}

void free(void *ptr)
{
    for (int i = 0; blocks[i].present; i++)
        if (blocks[i].used == false && blocks[i].addr == ptr)
            blocks[i].used = true;
}

size_t init_memory()
{
    heap_ptr = 0x1000000;
    memset(blocks, 0, sizeof(block) * 1024);
}