#include "vfs/tarfs.h"

#include <modules.h>

#ifdef MODULE_TARFS

#include <memory.h>
#include <string.h>
#include <stdio.h>
#include "tar.h"
#include <debug.h>
#include <devtab.h>
#include <mtab.h>

typedef struct fmem fmem;
struct fmem
{
	uint8_t* data;
	size_t size;
};

// file_interface* filler functions

int getc_memory(file_interface* stream)
{
	if(!stream)
		return -1;
	fmem* to = (fmem*)(stream->data);
	return (stream->position > to->size ? EOF : (to->data[stream->position++]));
}

int putc_memory(file_interface* stream, char c)
{
	if(!stream)
		return -1;
	fmem* to = (fmem*)(stream->data);
	return (stream->position > to->size ? EOF : (to->data[stream->position++] = c));
}

int seek_memory(file_interface* stream, off_t to, int from)
{
	if(!stream)
		return EOF;
	long int npos = 0;
	fmem* f = (fmem*)(stream->data);

	switch(from)
	{
		case SEEK_SET:
			npos = to;
			break;
		case SEEK_END:
			npos = f->size + to;
			break;
		case SEEK_CUR:
			npos = stream->position + to;
			break;
		default:
			return EOF;
	}
	if(npos < 0 || npos >= (long int)(f->size))
		return EOF;
	stream->position = npos;
	return 0;
}

// FILE* filler functions

int16_t fgetc_memory(FILE* stream)
{
	if(!stream)
		return -1;
	fmem* to = (fmem*)(stream->data);
	return (stream->position > to->size ? EOF : (to->data[stream->position++]));
}

int16_t fputc_memory(FILE* stream, char c)
{
	if(!stream)
		return -1;
	fmem* to = (fmem*)(stream->data);
	return (stream->position > to->size ? EOF : (to->data[stream->position++] = c));
}

int fseek_memory(FILE* stream, off_t to, int from)
{
	if(!stream)
		return EOF;
	long int npos = 0;
	fmem* f = (fmem*)(stream->data);

	switch(from)
	{
		case SEEK_SET:
			npos = to;
			break;
		case SEEK_END:
			npos = f->size + to;
			break;
		case SEEK_CUR:
			npos = stream->position + to;
			break;
		default:
			return EOF;
	}
	if(npos < 0 || npos >= (long int)(f->size))
		return EOF;
	stream->position = npos;
	return 0;
}

int open_fmem(fmem* to, file_interface* iface)
{
	memset(iface, 0, sizeof(file_interface));
	iface->data = (void*)(to);
	iface->getc = getc_memory;
	iface->putc = putc_memory;
	iface->seek = seek_memory;
	iface->opened = 1;
	return 1;
}

FILE* fopen_fmem(fmem* to)
{
	FILE* result = calloc(sizeof(FILE), 1);
	result->data = (void*)(to);
	result->getc = fgetc_memory;
	result->putc = fputc_memory;
	result->seek = fseek_memory;
	return result;
}

FILE* fopen_memory(void* addr, size_t size)
{
	fmem* to = calloc(sizeof(fmem), 1);
	to->data = addr;
	to->size = size;
	return fopen_fmem(to);
}

FILE* fopen_devtarfs(devtab_entry* ent)
{
	fmem* tar = (fmem*)(ent->data);
	return fopen_fmem(tar);
}

int open_devtarfs(devtab_entry* ent, int UNUSED(o), mode_t UNUSED(mode), file_interface* fiface)
{
	fmem* tar = (fmem*)(ent->data);
	return open_fmem(tar, fiface);
}

size_t expand(size_t num, size_t chunk)
{
	return num % chunk ? num + chunk - num % chunk : num;
}

tar_hdr_t* tar_find(FILE* devf, const char* name, int* id)
{
	fmem* fm = (fmem*)(devf->data);
	int i, n = tar_gethdrcnt(fm->data), nlen = strlen(name);
	tar_hdr_t* curr = (tar_hdr_t*)(fm->data);
	for(i = 0; i < n; ++i)
	{
		if(!strcmp(curr->name, name) || (!strncmp(curr->name, name, nlen) && curr->name[nlen] == '/' && !curr->name[nlen + 1]))
		{
			if(id)
				*id = i;
			return curr;
		}
		size_t fsize = tar_getsize(curr->size);
		curr = (tar_hdr_t*)((uint8_t*)(curr) + 512 + expand(fsize, 512));
	}
	return 0;
}

int tarfs_stat(FILE* devf, fs_interface* UNUSED(iface), const char* path, struct stat* to)
{
	if(!*path)
	{
		to->st_mode = S_IFDIR;
		to->st_size = 0;
		to->st_nlink = 1;
		return 0;
	}
	int id;
	tar_hdr_t* target = tar_find(devf, path, &id);
	if(!target)
		return -1;
	to->st_dev = devf->device;
	to->st_mode = /*target->mode*/0;
	to->st_nlink = 1;
	to->st_ino = id;
	to->st_size = tar_getsize(target->size);
	switch(target->typeflag)
	{
		case TAR_TYPE_DIR: to->st_mode |= S_IFDIR; break;
		case TAR_TYPE_REG: to->st_mode |= S_IFREG; break;
		default: return -1;
	}
	return 0;
}

DIR* tarfs_opendir(FILE* devf, fs_interface* UNUSED(iface), const char* path)
{
	if(*path && !tar_find(devf, path, 0))
		return 0;
	DIR* result = calloc(sizeof(DIR), 1);
	int len = strlen(path);
	if(len && path[len - 1] == '/')
		--len;

	fmem* fm = (fmem*)(devf->data);
	int i, n = tar_gethdrcnt(fm->data);
	tar_hdr_t* hdr = (tar_hdr_t*)(fm->data);
	struct dirent* curr;
	struct dirent* prev = 0;
	for(i = 0; i < n; ++i)
	{
		if(!len || (!strncmp(hdr->name, path, len) && hdr->name[len] == '/'))
		{
			const char* z = strchr(hdr->name + len + !!len, '/');
			size_t tlen = strlen(hdr->name) - len - !!len - !!z;
			if((!z || !z[1]) && tlen)
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

				char* sn = calloc(tlen + 1, 1);
				strncpy(sn, hdr->name + len + !!len, tlen);
				curr->d_name = sn;
				curr->d_ino  = i;
			}
		}
		size_t fsize = tar_getsize(hdr->size);
		hdr = (tar_hdr_t*)((uint8_t*)(hdr) + 512 + expand(fsize, 512));
	}
	return result;
}

// This thing is used for fopen a file inside the FS
FILE* tarfs_fopen(FILE* devf, fs_interface* UNUSED(iface), const char* path, const char* UNUSED(mode))
{
	tar_hdr_t* curr = tar_find(devf, path, 0);
	if(!curr)
		return 0;
	fmem* t = calloc(sizeof(fmem), 1);
	t->data = (uint8_t*)(curr) + 512;
	t->size = tar_getsize(curr->size);
	return fopen_fmem(t);
}

int tarfs_open(FILE* devf, fs_interface* UNUSED(iface), const char* path, int UNUSED(flags), mode_t UNUSED(mode), file_interface* fiface)
{
	tar_hdr_t* curr = tar_find(devf, path, 0);
	if(!curr)
		return -1;
	fmem* t = calloc(sizeof(fmem), 1);
	t->data = (uint8_t*)(curr) + 512;
	t->size = tar_getsize(curr->size);
	return open_fmem(t, fiface);
}

void init_rootfs()
{
	struct mboot_info* mboot = kernel_mboot_info;
	struct mboot_mod_list_struct* mboot_mod_list;
	mboot_mod_list = (struct mboot_mod_list_struct*) (uintptr_t) mboot->mods_addr;
	unsigned int i;
	fmem* s = 0;
	for (i = 0; i <= (mboot->mods_count); i++)
	{
		if(strcmp((const char *)mboot_mod_list->cmdline, "initrd")==0) //initrd mark comparsion
		{
			s = calloc(sizeof(fmem), 1);
			s->data = (void*)(size_t)(mboot_mod_list->mod_start);
			s->size = mboot_mod_list->mod_end - mboot_mod_list->mod_start;
			break;
		}
		mboot_mod_list++;
	}
	if(!s || !s->data)
		return;

	// Initialization
	fs_interface* if_tarfs = calloc(sizeof(fs_interface), 1);
	if_tarfs->stat       = tarfs_stat;
	if_tarfs->opendir    = tarfs_opendir;
	if_tarfs->open_file  = tarfs_open;
	if_tarfs->fopen_file = tarfs_fopen;
	if_tarfs->name       = "tarfs";
	// Insert functions

	devtab_entry d;
	d.file  = "initrd";
	d.name  = "initrd";
	d.fopen = fopen_devtarfs;
	d.open  =  open_devtarfs;
	d.data  = s;
	dev_t dev = add_dev(&d);
	mount_fs(dev, "/", if_tarfs);
	mount_fs(dev, "/storage/", if_tarfs);
}

#endif
