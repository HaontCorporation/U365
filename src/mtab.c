#include <mtab.h>

#include <memory.h>
#include <debug.h>
#include <io.h>
#include <stdio.h>
#include <fs.h>

mtab __mtab;

void mtab_init()
{
	__mtab._end = calloc(sizeof(mtab_entry), 1);
	__mtab._end->_prev = __mtab._end;
	__mtab._end->_next = __mtab._end;
}

void ins_mnt(mtab_entry* obj)
{
	obj->_prev = __mtab._end->_prev;
	obj->_next = __mtab._end;
	__mtab._end->_prev->_next = obj;
	__mtab._end->_prev = obj;
}

void add_mnt(mtab_entry* obj)
{
	mtab_entry* ins = calloc(sizeof(mtab_entry), 1);
	memcpy(ins, obj, sizeof(mtab_entry));
	ins_mnt(ins);
}

void rm_mnt(mtab_entry *val)
{
	val->_prev->_next = val->_next;
	val->_next = val->_prev;
}

void ls_mnt()
{
	mtab_entry* curr;
	int i = 0; // Just in case something has gone wrong
	for(curr = __mtab._end->_next; curr && curr != __mtab._end && i++ < 15; curr = curr->_next)
	{
		devtab_entry* fnd = find_dev(curr->dev);
		if(!fnd)
			printf("Error: wrong device\n");
		printf("Device: /dev/%s: \"%s\" (type: %s)\n", fnd->file, curr->path, curr->iface->name);
	}
}

int mount_fs(dev_t dev, const char* path, fs_interface* iface)
{
	// Check the data!!!
	mtab_entry* ins = calloc(sizeof(mtab_entry), 1);
	char* ns = calloc(1, strlen(path) + 1);
	int n = strlen(path);
	for(; n > 1 && path[n - 1] == '/'; --n);
	strncpy(ns, path, n);
	ins->dev = dev;
	ins->iface = iface;
	ins->path = ns;
	ins_mnt(ins);
	return 0;
}

int mount_find(const char* path, const char** find, dev_t* dev, fs_interface** iface)
{
	int mlen = 0, len;
	mtab_entry* curr;
	mtab_entry* best = 0;

	for(curr = __mtab._end->_next; curr != __mtab._end; curr = curr->_next)
	{
		len = strlen(curr->path);
		if(len > mlen && !strncmp(path, curr->path, len))
		{
			mlen = len;
			best = curr;
		}
	}
	if(best)
	{
		if(find)  *find  = path + mlen + (path[mlen] == '/');
		if(dev)   *dev   = best->dev;
		if(iface) *iface = best->iface;
		return 0;
	}
	return -1;
}

int stat(const char* path, struct stat* to)
{
	const char* nl;
	dev_t dev;
	fs_interface* iface;
	char* rpath = expand_path(path);
	if(mount_find(rpath, &nl, &dev, &iface) == -1)
		return -1;
	FILE* f = open_dev(dev, "r");
	if(!f)
	{
		printf("Failed to open device %i\n", dev);
		return -1;
	}
	return fs_stat(f, iface, nl, to);
}
