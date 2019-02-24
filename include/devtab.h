#ifndef DEVTAB_H
#define DEVTAB_H

#include <modules.h>

#ifdef MODULE_DEVTAB

#include <stdint.h>

typedef struct devtab_entry devtab_entry;
typedef uint16_t dev_t;

#include <fio.h>
#include <proctab.h>

struct devtab_entry
{
	devtab_entry* _next;
	devtab_entry* _prev;

	const char* name; // short device name
	const char* description;
	const char* file; // udev or so on
	uint16_t vendorID;
	uint16_t deviceID;
	dev_t id;
	int type; // 0 for character, 1 for block, 2 for filesystem

	void* data; // custom device data for any driver's internal purposes

	// DEPRECATED
	FILE* (*fopen)(devtab_entry*);
	int   (* open)(devtab_entry*, int, mode_t, file_interface*);

	int readable : 1;
	int writable : 1;
};

struct devtab
{
	devtab_entry* _end;
};

extern struct devtab __devtab;
extern        dev_t  __devtab_next;

void               devtab_init();
dev_t          ins_dev        (devtab_entry* val);
dev_t          add_dev        (devtab_entry* val);
void            rm_dev        (devtab_entry* val);
dev_t          add_dev_port   (uint16_t port, const char* name);
int             ls_dev        ();
devtab_entry* find_dev        (dev_t id);
FILE*             dev_open    (devtab_entry* obj, const char* mask);
FILE*         open_dev        (dev_t id,          const char* mask);
FILE* unused_fopen(devtab_entry* ent);
int   unused_open(devtab_entry* ent, int o, mode_t mode, file_interface* fiface);

#endif // MODULE_DEVTAB

#endif // DEVTAB_H
