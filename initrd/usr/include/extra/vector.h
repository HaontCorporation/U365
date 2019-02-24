#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stddef.h>

typedef struct vector_entry vector_entry;
typedef struct vector vector;

struct vector
{
	void** begin;
	void** end;
	void** alloc;
	void*  (*build)();
	void   (*del)(void*);
};

// Basic class operations
vector* vector_new();
void    vector_delete        (vector* obj);
uint8_t vector_constructor   (vector* obj);
void    vector_destructor    (vector* obj);

// Methods
void**  vector_at            (vector* obj, size_t p);
void    vector_resize        (vector* obj, size_t s);
void    vector_realloc       (vector* obj, size_t s);
void    vector_clear         (vector* obj);
void    vector_push_back     (vector* obj, void* e);
void    vector_pop_back      (vector* obj);
size_t  vector_size          (vector* obj);
size_t  vector_allocated     (vector* obj);
void    vector_erase_by_value(vector* obj, void* e);
void    vector_erase_by_id   (vector* obj, size_t id);
size_t  vector_find          (vector* obj, void* e);

#endif
