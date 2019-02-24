#include <devtab.h>

#include <memory.h>
#include <debug.h>
#include <io.h>
#include <stdio.h>
#include <fs.h>
#include <time.h>

struct devtab __devtab;
dev_t __devtab_next = 1;

dev_t ins_dev(devtab_entry* obj)
{
	obj->_prev = __devtab._end->_prev;
	obj->_next = __devtab._end;
	__devtab._end->_prev->_next = obj;
	__devtab._end->_prev = obj;
	return (obj->id = __devtab_next++);
}

dev_t add_dev(devtab_entry* obj)
{
	devtab_entry* ins = calloc(sizeof(devtab_entry), 1);
	memcpy(ins, obj, sizeof(devtab_entry));
	return ins_dev(ins);
}

void rm_dev(devtab_entry *val)
{
	val->_prev->_next = val->_next;
	val->_next->_prev = val->_prev;
}

devtab_entry* find_dev(dev_t id)
{
	devtab_entry* curr;
	for(curr = __devtab._end->_next; curr != __devtab._end; curr = curr->_next)
	{
		if(curr->id == id)
			return curr;
	}
	return 0;
}

FILE* dev_open(devtab_entry* ent, const char* UNUSED(mask))
{
	if(ent && ent->fopen)
		return ent->fopen(ent);
	return 0;
}

FILE* open_dev(dev_t id, const char* mask)
{
	devtab_entry* ent = find_dev(id);
	if(ent)
		return dev_open(ent, mask);
	return 0;
}

int16_t getc_kbd(FILE* UNUSED(d))
{
	return getchar();
}

struct dev_ps2m_mouse_packet {
	int16_t movement_x;
	int16_t movement_y;
	uint8_t button_l;
	uint8_t button_m;
	uint8_t button_r;
} ps2m_buffer;
static uint8_t ps2m_buffer_position;

#ifndef REDEPREC

int16_t dev_port_fgetc(FILE* obj)
{
	devtab_entry* ent = (devtab_entry*)(obj->data);
	return inb((uint16_t)(size_t)(ent->data));
}

int16_t dev_port_fputc(FILE* obj, char ch)
{
	devtab_entry* ent = (devtab_entry*)(obj->data);
	outb((uint16_t)(size_t)(ent->data), ch);
	return ch;
}

FILE* dev_port_fopen(devtab_entry* ent)
{
	FILE* result = calloc(sizeof(FILE), 1);
	result->data = ent->data;
	result->getc = dev_port_fgetc;
	result->putc = dev_port_fputc;
	return result;
}

int16_t fgetc_ps2m(FILE* UNUSED(d))
{
	if(ps2m_buffer_position == sizeof(ps2m_buffer) - 1)
		ps2m_buffer_position = 0;
	return ((uint8_t*)&ps2m_buffer)[ps2m_buffer_position++];
}

int16_t fgetc_null(FILE* UNUSED(d))
{
	return EOF;
}

int16_t fgetc_zero(FILE* UNUSED(d))
{
	return 0;
}

int16_t fgetc_urnd(FILE* UNUSED(d))
{
	return (char)(rand());
}

int16_t fputc_null(FILE* UNUSED(d), char c)
{
	return c;
}

FILE* dev_null_fopen(devtab_entry* UNUSED(ent))
{
	FILE* result = calloc(sizeof(FILE), 1);
	result->getc = fgetc_null;
	result->putc = fputc_null;
	return result;
}

FILE* dev_zero_fopen(devtab_entry* UNUSED(ent))
{
	FILE* result = calloc(sizeof(FILE), 1);
	result->getc = fgetc_zero;
	result->putc = fputc_null;
	return result;
}

FILE* dev_urnd_fopen(devtab_entry* UNUSED(ent))
{
	FILE* result = calloc(sizeof(FILE), 1);
	result->getc = fgetc_urnd;
	result->putc = fputc_null;
	return result;
}

FILE* dev_ps2m_fopen(devtab_entry* UNUSED(ent))
{
	FILE* result = calloc(sizeof(FILE), 1);
	result->getc = fgetc_ps2m;
	result->putc = fputc_null;
	return result;
}
#endif

// file_interface-based functions

int dev_port_getc(file_interface* obj)
{
	devtab_entry* ent = (devtab_entry*)(obj->data);
	return inb((uint16_t)(size_t)(ent->data));
}

int dev_port_putc(file_interface* obj, char ch)
{
	devtab_entry* ent = (devtab_entry*)(obj->data);
	outb((uint16_t)(size_t)(ent->data), ch);
	return ch;
}

int dev_port_open(devtab_entry* ent, int UNUSED(o), mode_t UNUSED(mode), file_interface* fiface)
{
	fiface->data = ent->data;
	fiface->getc = dev_port_getc;
	fiface->putc = dev_port_putc;
	return 1;
}

int getc_ps2m(file_interface* UNUSED(d))
{
	if(ps2m_buffer_position == sizeof(ps2m_buffer) - 1)
		ps2m_buffer_position = 0;
	return ((uint8_t*)&ps2m_buffer)[ps2m_buffer_position++];
}

int getc_null(file_interface* UNUSED(d))
{
	return EOF;
}

int getc_zero(file_interface* UNUSED(d))
{
	return 0;
}

int getc_urnd(file_interface* UNUSED(d))
{
	return (char)(rand());
}

int putc_null(file_interface* UNUSED(d), char c)
{
	return c;
}

int dev_null_open(devtab_entry* UNUSED(ent), int UNUSED(o), mode_t UNUSED(mode), file_interface* fiface)
{
	fiface->getc = getc_null;
	fiface->putc = putc_null;
	fiface->opened = 1;
	return 1;
}

int dev_zero_open(devtab_entry* UNUSED(ent), int UNUSED(o), mode_t UNUSED(mode), file_interface* fiface)
{
	fiface->getc = getc_zero;
	fiface->putc = putc_null;
	fiface->opened = 1;
	return 1;
}

int dev_urnd_open(devtab_entry* UNUSED(ent), int UNUSED(o), mode_t UNUSED(mode), file_interface* fiface)
{
	fiface->getc = getc_urnd;
	fiface->putc = putc_null;
	fiface->opened = 1;
	return 1;
}

int dev_ps2m_open(devtab_entry* UNUSED(ent), int UNUSED(o), mode_t UNUSED(mode), file_interface* fiface)
{
	fiface->getc = getc_ps2m;
	fiface->putc = putc_null;
	fiface->opened = 1;
	return 1;
}

// Inserters

dev_t add_dev_port(uint16_t port, const char* name)
{
	devtab_entry* ins = calloc(sizeof(devtab_entry), 1);
	char* sn = calloc(1, strlen(name) + 1);
	strcpy(sn, name);
	ins->data  = (void*)(size_t)(port);
	ins->name  = sn;
	ins->file  = sn;
	ins->fopen = dev_port_fopen;
	ins->open  = dev_port_open;
	ins->readable = 1;
	ins->writable = 1;
	ins_dev(ins);
	return ins->id;
}

int ls_dev()
{
	devtab_entry* curr = __devtab._end->_next;
	while(curr && curr != __devtab._end)
	{
		printf("[%i] /dev/%s: \"%s\"\n", (int)(curr->id), curr->file, curr->name);
		curr = curr->_next;
	}
	return 0;
}

devtab_entry* find_dev_by_name(const char* name)
{
	devtab_entry* curr;
	for(curr = __devtab._end->_next; curr != __devtab._end; curr = curr->_next)
	{
		if(!strcmp(curr->file, name))
		{
			return curr;
		}
	}
	return 0;
}

int devfs_stat(FILE* fs, fs_interface* UNUSED(iface), const char* path, struct stat* to)
{
	if(*path)
	{
		devtab_entry* ent = find_dev_by_name(path);
		if(!ent)
			return -1;
		to->st_rdev  = ent->id;
		to->st_ino   = ent->id;
		to->st_mode  = S_IFCHR;
	}
	else
	{
		to->st_rdev = 0;
		to->st_ino  = 0;
		to->st_mode = S_IFDIR;
	}
	to->st_nlink = 1;
	to->st_size  = 0;
	to->st_dev   = (dev_t)(size_t)(fs->data);
	return 0;
}

DIR* devfs_opendir(FILE* UNUSED(fs), fs_interface* UNUSED(iface), const char* path)
{
	if(*path)
		return 0;

	DIR* result = calloc(sizeof(DIR), 1);
	struct dirent* curr = 0;
	struct dirent* prev = 0;
	devtab_entry* ent = 0;
	for(ent = __devtab._end->_next; ent != __devtab._end; ent = ent->_next)
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
		char* sn = calloc(strlen(ent->file) + 1, 1);
		strcpy(sn, ent->file);
		curr->d_name = sn;
		curr->d_ino  = ent->id;
	}

	return result;
}

// DEPRECATED
FILE* devfs_fopen(FILE* UNUSED(fs), fs_interface* UNUSED(iface), const char* path, const char* UNUSED(mode))
{
	devtab_entry* ent = find_dev_by_name(path);
	if(ent && ent->fopen)
		return ent->fopen(ent);
	return 0;
}

int devfs_open(FILE* UNUSED(fs), fs_interface* UNUSED(iface), const char* path, int o, mode_t mode, file_interface* fiface)
{
	devtab_entry* ent = find_dev_by_name(path);
	if(ent && ent->open)
		return ent->open(ent, o, mode, fiface);
	return 0;
}

FILE* unused_fopen(devtab_entry* UNUSED(ent))
{
	FILE* result = calloc(sizeof(FILE), 1);
	return result;
}

int unused_open(devtab_entry* UNUSED(ent), int UNUSED(o), mode_t UNUSED(mode), file_interface* UNUSED(fiface))
{
	return 0;
}

void devtab_init()
{
	__devtab._end = calloc(sizeof(devtab_entry), 1);
	__devtab._end->_prev = __devtab._end;
	__devtab._end->_next = __devtab._end;

	// Insert a loop device for devtab reading
	// Consists of special devices only
	// Currently not working
	// Replace it later with add_dev_fs(devfs, "devfs");
	//Filesystem* devfs = calloc(sizeof(Filesystem), 1);
	//devfs->
	//add_dev_fs(devfs, "devfs");
	devtab_entry* dev_null = calloc(sizeof(devtab_entry), 1);
	devtab_entry* dev_zero = calloc(sizeof(devtab_entry), 1);
	devtab_entry* dev_urnd = calloc(sizeof(devtab_entry), 1);
	devtab_entry* dev_ps2m = calloc(sizeof(devtab_entry), 1);
	dev_null->name = "null device";
	dev_null->file = "null";
	dev_zero->name = "zero device";
	dev_zero->file = "zero";
	dev_urnd->name = "Pseudo-random generator";
	dev_urnd->file = "urandom";
	dev_ps2m->name = "PS/2 mouse device. READ ONLY AS PACKETS, BETTER DON'T DO IT AT ALL OR THE ENTIRE SYSTEM WOULD BE FUCKED";
	dev_ps2m->file = "mouse";
	dev_null->fopen = dev_null_fopen;
	dev_zero->fopen = dev_zero_fopen;
	dev_urnd->fopen = dev_urnd_fopen;
	dev_ps2m->fopen = dev_ps2m_fopen;
	dev_null-> open = dev_null_open;
	dev_zero-> open = dev_zero_open;
	dev_urnd-> open = dev_urnd_open;
	dev_ps2m-> open = dev_ps2m_open;
	ins_dev(dev_null);
	ins_dev(dev_zero);
	ins_dev(dev_urnd);
	ins_dev(dev_ps2m);

	fs_interface* devfsiface = calloc(sizeof(fs_interface), 1);
	devtab_entry* devfsdev   = calloc(sizeof(devtab_entry), 1);
	devfsiface->stat       = devfs_stat;
	devfsiface->opendir    = devfs_opendir; // Remove
	devfsiface->fopen_file = devfs_fopen;   // Remove
	devfsiface-> open_file = devfs_open;
	devfsiface->name       = "devfs";
	devfsdev->file         = "devfs";
	devfsdev->name         = "devtab FS";
	devfsdev->fopen        = unused_fopen;
	devfsdev->open         = unused_open;
	ins_dev(devfsdev);
	iftab_add(devfsiface);
	mount_fs(devfsdev->id, "/dev", devfsiface);

#ifdef MODULE_i686
	// COM & LPT ports, if exist
	char names[7][7] = {"com1", "com2", "com3", "com4", "lpt1", "lpt2", "lpt3"};
	uint16_t* d = (uint16_t*)(0x400);
	for(uint8_t i = 0; i < 7; ++i)
	{
		if(d[i])
		{
			printk_on = 1;
			printk("Port %s detected", names[i]);
			printk_on = 0;
			add_dev_port(d[i], names[i]);
		}
	}
#endif
}


