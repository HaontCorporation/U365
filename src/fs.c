#include <fs.h>

#include <stdio.h>
#include <errno.h>
#include <mtab.h>
#include <env.h>

FILE *opened_files[2048] = {};
unsigned int opened_files_count = 3;
//uint64_t device_number = 1;
struct iftab __iftab;

int fs_create_file(FILE* obj, fs_interface* iface, const char* name, mode_t flags)
{
	if(obj && iface && name && iface->create_file)
		return iface->create_file(obj, iface, name, flags);
	return -1;
}

int fs_delete_file(FILE* obj, fs_interface* iface, const char* name)
{
	if(obj && iface && name && iface->delete_file)
		return iface->delete_file(obj, iface, name);
	return -1;
}

int fs_create_dir(FILE* obj, fs_interface* iface, const char* name, mode_t flags)
{
	if(obj && iface && name && iface->create_dir)
		return iface->create_dir(obj, iface, name, flags);
	return -1;
}

int fs_delete_dir(FILE* obj, fs_interface* iface, const char* name)
{
	if(obj && iface && name && iface->delete_dir)
		return iface->delete_dir(obj, iface, name);
	return -1;
}

int fs_open_file(FILE* obj, fs_interface* iface, const char* path, int flags, mode_t mode, file_interface* fiface)
{
	if(obj && iface && path && iface->open_file)
	{
		memset(fiface, 0, sizeof(fiface));
		return iface->open_file(obj, iface, path, flags, mode, fiface);
	}
	errno = ENOENT;
	return -1;
}

FILE* fs_fopen_file(FILE* obj, fs_interface* iface, const char* path, const char* mode)
{
	if(obj && iface && path && iface->fopen_file)
	{
		FILE* ret = iface->fopen_file(obj, iface, path, mode);
		//opened_files[opened_files_count] = ret;
		//ret->fd = opened_files_count;
		//opened_files_count++;
		return ret;
	}
	errno = ENOENT;
	return 0;
}

DIR* fs_opendir(FILE* obj, fs_interface* iface, const char* path)
{
	if(obj && iface && path && iface->opendir)
		return iface->opendir(obj, iface, path);
	return 0;
}

int fs_stat(FILE* obj, fs_interface* iface, const char* path, struct stat* to)
{
	if(obj && iface && path && iface->stat)
		return iface->stat(obj, iface, path, to);
	return -1;
}

// Now, high-level functions
DIR* opendir(const char* path)
{
	struct stat to;
	//if(stat(path, &to))
		//return 0;

	char* rpath = expand_path(path);
	const char* ln;
	dev_t dev;
	fs_interface* iface;
	if(mount_find(rpath, &ln, &dev, &iface))
		return 0;

	//printf("Mount node found, device %i, type \"%s\"\n", (int)(dev), iface->name);
	FILE* f = open_dev(dev, "r");
	if(!f || fs_stat(f, iface, ln, &to))
		return 0;

	if(S_ISDIR(to.st_mode))
		return fs_opendir(f, iface, ln);
	return 0;
}

struct dirent* readdir(DIR* obj)
{
	struct dirent* result = obj->current;
	if(obj->current)
		obj->current = obj->current->next;
	return result;
}

// Good example of fs_* usage
int fs_list_files(const char* path, ls_flags_t flags)
{
	uint8_t show_color   = 0;
	uint8_t show_symbols = 0;
	uint8_t show_hidden  = 0;
	uint8_t show_backups = 0;
	if(flags & LS_COLOR)   show_color   = 1;
	if(flags & LS_SYMBOLS) show_symbols = 1;
	if(flags & LS_HIDDEN)  show_hidden  = 1;
	if(flags & LS_BACKUPS) show_backups = 1;

	DIR* dir = opendir(path); // We shouldn't care about relative/absolute paths at this state
	if(!dir)
	{
		//char* rpath = expand_path(path);
		printf("Failed to opendir \"%s\"\n", path);
		return -1;
	}

	int maxlen = 0, tlen = strlen(path);
	struct dirent* ent;
	while((ent = readdir(dir)))
	{
		char* dname = ent->d_name;
		if(!dname) continue; // Something very bad happened...
		if(dname[0] == '.' && (!dname[1] || (dname[1] == '.' && !dname[2]))) continue; // Skip "." and ".."

		int len = strlen(dname);
		if(len > maxlen)
			maxlen = len;
	}
	dir->current = dir->first;

	char* buf = calloc(maxlen + tlen + 2, 1);
	strcpy(buf, path);
	if(buf[tlen - 1] != '/')
	{
		buf[tlen++] = '/';
	}
	struct stat to;

	if(flags & LS_POINTS) printf(".\n..\n");
	while((ent = readdir(dir)))
	{
		char* dname = ent->d_name;
		if(!dname) continue; // Something very bad happened...
		if(dname[0] == '.' && (!dname[1] || (dname[1] == '.' && !dname[2]))) continue; // Skip "." and ".."

		strcpy(buf + tlen, dname);
		stat(buf, &to);

		//printf("[%6i]: %s\n",ent->d_ino, dname);

		size_t len = strlen(dname);
		//uint8_t is_dir    = 0;
		uint8_t is_hidden = 0;
		uint8_t is_backup = 0;
		//if(fs_is_dir(dname))    is_dir = 1;
		//if(S_ISDIR(to.st_mode))   is_dir    = 1;
		if(dname[0] == '.')       is_hidden = 1;
		if(dname[len - 1] == '~') is_backup = 1;

		if((!is_hidden || show_hidden) && (!is_backup || show_backups))
		{
			if(show_color)
			{
				switch(to.st_mode & S_IFMT)
				{
					case S_IFREG: printf("\e[1;32m"); break;
					case S_IFDIR: printf("\e[1;34m"); break;
					case S_IFCHR: printf("\e[1;33m"); break;
					case S_IFLNK: printf("\e[1;36m"); break;
				}
			}
			printf("%s", dname);
			if(show_color) printf("\e[0m");
			if(show_symbols)
			{
				switch(to.st_mode & S_IFMT)
				{
					case S_IFDIR: printf("/"); break;
					case S_IFLNK: printf("@"); break;
				}
			}
			printf("\n");
		}
	}
	return 0;
}

void iftab_add(fs_interface* obj)
{
	iftab_entry* ent = calloc(sizeof(iftab_entry), 1);
	ent->iface = obj;
	ent->_prev = __iftab._end->_prev;
	ent->_next = __iftab._end;
	__iftab._end->_prev->_next = ent;
	__iftab._end->_prev = ent;
}

void iftab_init()
{
	__iftab._end = calloc(sizeof(iftab_entry), 1);
	__iftab._end->_prev = __iftab._end;
	__iftab._end->_next = __iftab._end;
}

char* expand_path(const char* src)
{
	char* result = 0;
	const char* from = 0;

	if(*src == '~' && (!src[1] || src[1] == '/'))
	{
		from = get_env_variable("HOME");
		++src;
	}
	if(*src != '/' && !from)
		from = get_env_variable("PWD");

	result = calloc((from ? strlen(from) : 0) + strlen(src) + 2, 1);
	int pos = 0;
	if(from)
	{
		strcpy(result, from);

		pos = strlen(from);
		if(pos && result[pos - 1] != '/')
			result[pos++] = '/';
	}

	int i;
	for(i = 0; src[i] && (!i || src[i - 1]); ++i)
	{
		if(src[i] == '/' && result[pos - 1] == '/')
			continue;
		// If the name starts with '.'
		if(src[i] == '.' && result[pos - 1] == '/')
		{
			// If it is "."
			if(src[i + 1] == '/' || !src[i + 1])
			{
				i += 1;
				continue;
			}
			// If it is ".."
			if(src[i + 1] == '.' && (src[i + 2] == '/' || !src[i + 2]))
			{
				pos -= 2;
				for(; pos >= 0 && result[pos] != '/'; --pos)
				{
					result[pos] = 0;
				}
				if(pos < 0)
				{
					return 0;
				}
				++pos;
				i += 2;
				continue;
			}
		}
		result[pos++] = src[i];
		result[pos] = 0;
	}
	while(pos > 1 && result[pos - 1] == '/')
		result[--pos] = 0;
	return result;
}
