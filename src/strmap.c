#include <strmap.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>
#include <debug.h>

strmap* strmap_new()
{
	strmap* obj = calloc(sizeof(strmap), 1);
	return strmap_constructor(obj);
}

strmap* strmap_constructor(strmap* obj)
{
	obj->_end = calloc(sizeof(strmap_node), 1);
	obj->_end->parent = obj;
	obj->_end->prev = obj->_end;
	obj->_end->next = obj->_end;
	return obj;
}

void* strmap_get(strmap *obj, const char *key)
{
	strmap_node* node = strmap_fnd(obj, key);
	if(node) return node->data;
	return NULL;
}

void strmap_set(strmap* obj, const char* key, void* data)
{
	strmap_node* node = strmap_fnd(obj, key);
	if(node) node->data = data;
	else strmap_ins(obj, key, data);
}

void strmap_ins(strmap* obj, const char* key, void* data)
{
	strmap_node* node = calloc(sizeof(strmap_node), 1);
	char* name = calloc(strlen(key) + 1, 1);
	strcpy(name, key);
	node->parent = obj;
	node->data = data;
	node->name = name;

	node->prev = obj->_end->prev;
	node->next = obj->_end;

	obj->_end->prev = node;
	node->prev->next = node;
}

strmap_node* strmap_fnd(strmap* obj, const char* key)
{
	for(strmap_node* i = obj->_end->next; i != obj->_end; i = i->next)
	{
		if(!i || !i->name)
		{
			printf("Boom!\n");
			return NULL;
		}
		if(!strcmp(i->name, key))
		{
			return i;
		}
	}
	return NULL;
}

void strmap_node_del(strmap_node* obj)
{
	obj->prev->next = obj->next;
	obj->next->prev = obj->prev;
	//free(obj);
}

void strmap_del(strmap* obj, const char* key)
{
	strmap_node* node = strmap_fnd(obj, key);
	if(node) strmap_node_del(node);
}
