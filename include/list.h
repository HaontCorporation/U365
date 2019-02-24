#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include <stddef.h>

typedef struct list_node list_node;
typedef struct list list;
typedef struct list list_on_array;

struct list_node
{
    list* parent;
    list_node* prev;
    list_node* next;
    void* data;
};

struct list
{
    list_node* first;
    list_node* last;

    // The size field is changed internally by the nodes' code and should not be used externally
    size_t size;

    size_t block;
    void (*insert_before)(list_node*, void*);
    void (*insert_after )(list_node*, void*);
};

list_node* list_node_new();
void       list_node_delete();
uint8_t    list_node_constructor(list_node*);
void       list_node_destructor(list_node*);

list*      list_new();
void       list_delete();
uint8_t    list_constructor(list*);
uint8_t    list_destructor(list*);

list_node* list_node_prev(list_node*);
list_node* list_node_next(list_node*);
void       list_node_insert_before(list_node*, void*);
void       list_node_insert_after(list_node*, void*);
void       list_node_remove(list_node*);
list_node* list_node_find_next(list_node*, void*);

uint8_t    list_empty(list*);
list_node* list_first(list*);
list_node* list_last(list*);
void       list_push_back(list*, void*);
void       list_push_front(list*, void*);
list_node* list_find_first(list*, void*);

/**
 * List on array - a workaround for creating VFS, Allocator, or so.
 * DO NOT USE if you don't know what is it for and how to use it.
 */
void       list_on_array_constructor       (list_on_array*, size_t);
void       list_on_array_empty             (list_on_array*);
list_node* list_on_array_first             (list_on_array*);
list_node* list_on_array_last              (list_on_array*);
void       list_on_array_find_new          (list_on_array*);
void       list_on_array_node_insert_before(list_node*, void*);
void       list_on_array_node_insert_after (list_node*, void*);

#endif // LIST_H
