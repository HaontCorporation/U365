#ifndef __MEM_H
#define __MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

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
