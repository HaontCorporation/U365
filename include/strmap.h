#ifndef STRMAP_H
#define STRMAP_H

typedef struct strmap strmap;
typedef struct strmap_node strmap_node;

struct strmap_node
{
	strmap* parent;
	strmap_node* prev;
	strmap_node* next;
	const char* name;
	void* data;
};

struct strmap
{
	strmap_node* _end;
};

void         strmap_node_del(strmap_node* obj);
strmap*      strmap_new();
strmap*      strmap_constructor(strmap* obj);
void         strmap_ins(strmap* obj, const char* key, void* data);
void         strmap_set(strmap* obj, const char* key, void* data);
void*        strmap_get(strmap* obj, const char* key);
void         strmap_del(strmap* obj, const char* key);
strmap_node* strmap_fnd(strmap* obj, const char* key);


#endif // STRMAP_H
