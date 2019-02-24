#include <proctab.h>

#ifdef MODULE_PROCTAB

#include <memory.h>
#include <debug.h>
#include <io.h>
#include <stdio.h>
#include <fs.h>
#include <time.h>
#include <vfs/tarfs.h>

struct proctab __proctab;
uint32_t __proctab_next = 1;
volatile pid_t c_pid = 0;

uint32_t ins_proc(proctab_entry* obj)
{
	obj->_prev = __proctab._end->_prev;
	obj->_next = __proctab._end;
	__proctab._end->_prev->_next = obj;
	__proctab._end->_prev = obj;
	return (obj->pid = __proctab_next++);
}

uint32_t add_proc(proctab_entry* obj)
{
	proctab_entry* ins = calloc(sizeof(proctab_entry), 1);
	memcpy(ins, obj, sizeof(proctab_entry));
	return ins_proc(ins);
}

void rm_proc(proctab_entry *val)
{
	val->_prev->_next = val->_next;
	val->_next->_prev = val->_prev;
	free(val);
}

proctab_entry* find_proc(uint32_t id)
{
	proctab_entry* curr;
	for(curr = __proctab._end->_next; curr != __proctab._end; curr = curr->_next)
	{
		if(curr->pid == id)
			return curr;
	}
	return 0;
}

int procfs_stat(FILE* fs, fs_interface* UNUSED(iface), const char* path, struct stat* to)
{
	if(*path)
	{
		uint32_t tid = atoi(path);
		proctab_entry* ent = find_proc(tid);
		if(!ent)
			return -1;
		to->st_ino   = ent->pid;
		to->st_mode  = S_IFCHR;
	}
	else
	{
		to->st_ino  = 0;
		to->st_mode = S_IFDIR;
	}
	to->st_rdev = 0;
	to->st_nlink = 1;
	to->st_size  = 0;
	to->st_dev   = (dev_t)(size_t)(fs->data);
	return 0;
}

DIR* procfs_opendir(FILE* UNUSED(fs), fs_interface* UNUSED(iface), const char* path)
{
	if(*path)
		return 0;

	DIR* result = calloc(sizeof(DIR), 1);
	struct dirent* curr = 0;
	struct dirent* prev = 0;
	proctab_entry* ent = 0;
	for(ent = __proctab._end->_next; ent != __proctab._end; ent = ent->_next)
	{
		curr = calloc(sizeof(struct dirent), 1);
		if(!result->first)
		{
			result->first = curr;
			result->current = curr;
		}
		curr->prev = prev;
		if(prev)
			prev->next = curr;
		prev = curr;
		char* sn = calloc(20, 1);
		sprintf(sn, "%u", ent->pid);
		curr->d_name = sn;
		curr->d_ino  = ent->pid;
	}

	return result;
}

#ifndef REDEPREC
FILE* procfs_fopen(FILE* UNUSED(fs), fs_interface* UNUSED(iface), const char* UNUSED(path), const char* UNUSED(mode))
{
	//proctab_entry* ent = find_proc_by_name(path);
	//if(ent && ent->open)
		//return ent->open(ent);
	return 0;
}
#endif

void proctab_init()
{
	__proctab._end = calloc(sizeof(proctab_entry), 1);
	__proctab._end->_prev = __proctab._end;
	__proctab._end->_next = __proctab._end;

	proctab_entry testproc;
	add_proc(&testproc);
	add_proc(&testproc);


	fs_interface* procfsiface = calloc(sizeof(fs_interface), 1);
	devtab_entry* procfsdev   = calloc(sizeof(devtab_entry), 1);
	procfsiface->stat       = procfs_stat;
	procfsiface->opendir    = procfs_opendir;
	procfsiface->fopen_file = procfs_fopen;
	procfsiface->name       = "procfs";
	procfsdev->file         = "procfs";
	procfsdev->name         = "proctab FS";
	procfsdev->fopen        = unused_fopen;
	procfsdev-> open        = unused_open;
	ins_dev(procfsdev);
	iftab_add(procfsiface);
	mount_fs(procfsdev->id, "/proc", procfsiface);
}

#endif // MODULE_PROCTAB
