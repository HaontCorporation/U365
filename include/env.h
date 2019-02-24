#ifndef ENV_H
#define ENV_H

//#include <string_tree.h>
#include <strmap.h>

typedef struct env_descriptor env_descriptor;
struct env_descriptor
{
	//env_descriptor* parent; // Should be removed as soon as possible - copy the whole env, don't be a scroodge
	strmap* map;
};


const char* env_get(env_descriptor*, const char*);
void        env_set(env_descriptor*, const char*, const char*);
const char* get_env_variable(const char *);
void        set_env_variable(const char *, const char *);
void      setup_env();
void    display_env();

#endif
