#include <vector.h>

#include <stdio.h>
#include <memory.h>

// Basic class operations
vector* vector_new()
{
	vector* obj = malloc(sizeof(vector));
	if(obj && vector_constructor(obj))
		return obj;
	return 0;
}

void vector_delete(vector* obj)
{
	vector_destructor(obj);
	free(obj);
}

uint8_t vector_constructor(vector* obj)
{
	obj->begin = 0;
	obj->end   = 0;
	obj->alloc = 0;
	obj->build = 0;
	obj->del   = 0;
	return 1;
}

void vector_destructor (vector* obj)
{
	vector_clear(obj);
}

// Methods
void** vector_at(vector* obj, size_t num)
{
	if(num < vector_size(obj))
	{
		return &(obj->begin[num]);
	}
	else
	{
		perr("[VECTOR]: Vector subscript out of range\n");
		return 0;
	}
}

void vector_resize(vector* obj, size_t size)
{
	if(obj->del)
	{
		size_t oldsize = vector_size(obj);
		size_t i;
		for(i = size; i < oldsize; ++i)
		{
			obj->del(obj->begin[i]);
		}
	}
	if(vector_allocated(obj) < size)
	{
		vector_realloc(obj, size);
	}
	obj->end = obj->begin + size;
}

void vector_realloc(vector* obj, size_t size)
{
	size_t oldsize = vector_size(obj);
	obj->begin = realloc(obj->begin, sizeof(void*) * size);
	if(obj->begin)
	{
		obj->alloc = obj->begin + size;
		obj->end = obj->begin + oldsize;
		if(size < oldsize)
		{
			obj->end = obj->begin + size;
		}
	}
	else
	{
		perr("[VECTOR]: realloc() failed\n");
	}
}

void vector_clear(vector* obj)
{
	void** ptr;
	if(obj->del)
	{
		for(ptr = obj->begin; ptr != obj->end; ++ptr)
		{
			obj->del(ptr);
		}
	}
	free(obj->begin);
	memset(obj, 0, sizeof(vector));
}

size_t vector_size(vector* obj)
{
	return (size_t)(obj->end - obj->begin);
}

size_t vector_allocated(vector* obj)
{
	return (size_t)(obj->alloc - obj->begin);
}

void vector_push_back(vector* obj, void* elem)
{
	if(!obj->begin)
	{
		vector_realloc(obj, 4);
	}
	if(obj->end == obj->alloc)
	{
		vector_realloc(obj, vector_size(obj) * 2);
	}
	*(obj->end) = elem;
	++obj->end;
}

void vector_pop_back(vector* obj)
{
	if(obj->end > obj->begin)
	{
		if(obj->del)
		{
			obj->del(obj->end - 1);
		}
		--obj->end;
	}
	else
	{
		perr("[VECTOR]: Out of range\n");
	}
}

void vector_erase_by_value(vector* obj, void* val)
{
	vector_erase_by_id(obj, vector_find(obj, val));
}

void vector_erase_by_id(vector* obj, size_t i)
{
	size_t n = vector_size(obj) - 1;
	uint8_t f = 0;
	for(; i < n; ++i)
	{
		obj->begin[i] = obj->begin[i + 1];
		f = 1;
	}
	if(f)
	{
		vector_pop_back(obj);
	}
}

size_t vector_find(vector* obj, void* val)
{
	size_t i;
	size_t n = vector_size(obj);
	for(i = 0; i < n; ++i)
	{
		if(obj->begin[i] == val)
		{
			break;
		}
	}
	return i;
}