#include <string.h>
#include <env.h>
#include <memory.h>
#include <stdio.h>
#include <debug.h>

// Env descriptor for ISH
// Maybe we should move this to ish.c? // we shouldn't.
//env_descriptor *env;
env_descriptor __env;
uint8_t env_ready = 0;

void setup_env();

const char* env_get(env_descriptor* obj, const char* key)
{
	if(!env_ready)
	{
		printk("env_get() before setup_env(), aborting\n");
		return 0;
	}
	const char* result = (const char*)(strmap_get(obj->map, key));
	return result;
}

void env_set(env_descriptor* obj, const char* key, const char* value)
{
	char* buf = calloc(strlen(value) + 1, 1);
	strcpy(buf, value);
	strmap_set(obj->map, key, buf);
	char* z = strmap_get(obj->map, key);
	printk("Env \"%s\" is now set to \"%s\", while mentioned \"%s\"", key, z, buf);
}

void setup_env()
{
	//env = calloc(sizeof(env_descriptor), 1);
	__env.map = strmap_new();
	env_ready = 1;
	env_set(&__env, "USER",        "root");
	env_set(&__env, "HOSTNAME",    "localhost");
	env_set(&__env, "UID",         "0");
	env_set(&__env, "GID",         "0");
	env_set(&__env, "PWD",         "/");
	env_set(&__env, "HOME",        "/root");
	env_set(&__env, "PATH",        "/bin:/sbin:/usr/bin:/usr/sbin");
	env_set(&__env, "PS1",         "\\u@\\h : \\w \\$ ");
	env_set(&__env, "ISH_STARTUP", "");
	env_set(&__env, "ISH_PREEXEC", "");
	env_set(&__env, "GO",          "weird");
}
const char *get_env_variable(const char *var)
{
	return env_get(&__env, var);
}
void set_env_variable(const char *var, const char *val)
{
	return env_set(&__env, var, val);
}
void display_env()
{
	strmap_node* curr;
	for(curr = __env.map->_end->next; curr!= __env.map->_end; curr = curr->next)
	{
		printf("%s=\"%s\"\n", curr->name, (const char*)(curr->data));
	}
}
