#ifndef MTAB_H
#define MTAB_H

typedef struct stat stat_t;
typedef struct mtab mtab;
typedef struct mtab_entry mtab_entry;

#include <devtab.h>
#include <fio.h>
#include <fs.h>

struct mtab
{
	mtab_entry* _end;
};

struct mtab_entry
{
	dev_t         dev;
	const char*   path;
	fs_interface* iface;

	mtab_entry* _next;
	mtab_entry* _prev;
};

extern mtab __mtab;

void mtab_init();
void add_mnt(mtab_entry* val);
void  rm_mnt(mtab_entry* val);
void  ls_mnt();
int mount_fs(dev_t dev, const char* path, fs_interface* iface);
int mount_find(const char* path, const char** find, dev_t* dev, fs_interface** iface);

#endif // MTAB_H
