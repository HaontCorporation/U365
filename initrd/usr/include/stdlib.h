#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *malloc(size_t);
void* calloc(size_t, size_t);
void free(void*);

#ifdef __cplusplus
}
#endif

#endif
