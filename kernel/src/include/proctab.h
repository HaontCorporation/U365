#ifndef PROCTAB_H
#define PROCTAB_H

#include <modules.h>

#ifdef MODULE_PROCTAB

#include <stdint.h>

typedef struct proctab_entry proctab_entry;
typedef struct file_interface file_interface;
typedef uint32_t pid_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;

#include <vector.h>
#include <env.h>
#include <devtab.h>

struct proctab_entry
{
	proctab_entry* _next;
	proctab_entry* _prev;

	const char* name; // short device name
	const char* description;

	pid_t pid;
	uid_t uid;
	gid_t gid;
	vector* files;
	env_descriptor* env;
};

struct file_interface
{
	dev_t dev;
	const char* fname;
	void* data;
	int opened;
	size_t position;

	int (*getc )(file_interface* iface);
	int (*putc )(file_interface* iface, char c);
	int (*read )(file_interface* iface, void* targ, size_t size);
	int (*write)(file_interface* iface, void* targ, size_t size);
	int (*seek )(file_interface* iface, off_t offset, int origin);
	int (*close)(file_interface* iface);
};

struct proctab
{
	proctab_entry* _end;
};

extern struct proctab  __proctab;
extern        uint32_t __proctab_next;

void                proctab_init();
uint32_t        ins_proc        (proctab_entry* val);
uint32_t        add_proc        (proctab_entry* val);
void             rm_proc        (proctab_entry* val);
proctab_entry* find_proc        (uint32_t id);

extern volatile pid_t c_pid;

#endif // MODULE_PROCTAB

#endif // PROCTAB_H