#ifndef __MEM_H
#define __MEM_H
#include <stdint.h>
#include <stddef.h>

typedef struct __block block;
struct __block
{
    int present;
    void *addr;
    int size;
    int used;   
};

void* memcpy (void*, const void*, size_t);
void* memset (void*, uint8_t, size_t);
void* memmove(void*, void*, size_t);
// Old heap
void* malloc (int);
void* calloc (int, int);
void* realloc(void*, int);
void  free   (void*);
size_t init_memory();
#ifdef __cplusplus
}
#endif
#endif