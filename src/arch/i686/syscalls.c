#include <ish.h>
#include <tty_old.h>
#include <stdio.h>
#include <fs.h>
#include <panic.h>
#include <debug.h>
#include <memory.h>
#include <uname.h>
#include <env.h>
#include <arch/i686/syscalls.h>
#include <stdint.h>
#include <proctab.h>
#include <time.h>
#include <elf_hdr.h>
#include <graphics/surface.h>

file_interface* get_file_interface(int des)
{
	proctab_entry* ent = find_proc(c_pid);
	//printf("%i 0x%08x\n", c_pid, ent);
	if(!ent || !ent->files || (int)(vector_size(ent->files)) < des) return 0;
	file_interface* result = (file_interface*)*(vector_at(ent->files, des));
	return (result->opened ? result : 0);
}

// Also, maybe we need to also pass some kind of thread/process context?
uint32_t sc_read(int des, char* buf, size_t size)
{
	file_interface* iface = get_file_interface(des);
	if(!iface) return 0;
	if(iface->read)
		return iface->read(iface, buf, size);
	if(!iface->getc)
		return 0;
	// Fallback mode
	uint32_t i;
	for(i = 0; i < size; ++i)
	{
		int16_t c = iface->getc(iface);
		if(c == EOF)
			break;
		buf[i] = c;
	}
	return i;
}
uint32_t sc_write(int des, char* buf, size_t size)
{
	file_interface* iface = get_file_interface(des);
	//printf("%i 0x%08x\n", des, iface);
	if(!iface) return 0;
	if(iface->write)
		return iface->write(iface, buf, size);
	if(!iface->putc)
		return 0;
	// Fallback mode
	uint32_t i;
	for(i = 0; i < size; ++i)
	{
		int16_t c = iface->putc(iface, buf[i]);
		if(c == EOF)
			break;
	}
	return i;
}

uint32_t sc_open(const char* fname, int flags, mode_t mode)
{
	return open(fname, flags, mode);
}

uint32_t sc_close(int UNUSED(fd))
{
	return 0;
}

uint32_t sc_stat(const char* fname, struct stat* to)
{
	return stat(fname, to);
}

uint32_t sc_lseek(int des, off_t off, int o)
{
	file_interface* iface = get_file_interface(des);
	if(!iface) return -1;
	return iface->seek(iface, off, o);
}

uint32_t sc_uname(struct utsname* to)
{
	struct utsname uname = get_uname();
	memcpy(to, &uname, sizeof(struct utsname));
	return 0;
}

uint32_t sc_creat(const char* fname, mode_t mode)
{
	return sc_open(fname, 0, mode);
}

uint32_t sc_getcwd(char* buf, size_t size)
{
	// Change it for REAL process' cwd!
	extern env_descriptor __env;
	const char* cwd = env_get(&__env, "PWD");
	strncpy(buf, cwd, size);
	return 0;
}

// After-hole syscalls
int sc_execve(const char *filename, char** argv, char** envp)
{
    return run_elf_file(filename, argv, envp, 1);
}

// NON-STANDARD DEPRECATED SYSCALLS
// TEMP LIBRARY INTERNAL USAGE ONLY
// REMOVE THEM AS SOON AS POSSIBLE
// ok
uint32_t sc_t_alloc(size_t size)
{
	return (uint32_t)(malloc(size));
}
uint32_t sc_t_env_get(const char* name)
{
	extern env_descriptor __env;
	return (uint32_t)(env_get(&__env, name));
}
uint32_t sc_t_env_set(const char* name, const char* value)
{
	extern env_descriptor __env;
	env_set(&__env, name, value);
	return 0;
}

uint32_t sc_t_screen_apply(const surface* obj, rect r)
{
    return surface_apply_notransparency(surface_screen, obj, r);
}


void syscall_handler(void)
{
	uint32_t _eax, _ebx, _ecx, _edx, result = -1;
	asm volatile("cli" : "=a"(_eax), "=b"(_ebx), "=c"(_ecx), "=d"(_edx));
	uint32_t* argptr = (uint32_t*)(_ebx);
	if(argptr[0] != _eax)
		return;
	switch(_eax)
	{
		case SYS_read:      result = sc_read  ((int  )(argptr[1]), (char*)(argptr[2]), argptr[3]); break;
		case SYS_write:     result = sc_write ((int  )(argptr[1]), (char*)(argptr[2]), argptr[3]); break;
		case SYS_open:      result = sc_open  ((char*)(argptr[1]), (int  )(argptr[2]), argptr[3]); break;
		case SYS_close:     result = sc_close ((int  )(argptr[1]));                                break; // Still not implemented
		case SYS_stat:      result = sc_stat  ((char*)(argptr[1]), (void*)(argptr[2]));            break;

		case SYS_lseek:     result = sc_lseek ((int  )(argptr[1]), (off_t)(argptr[2]), argptr[3]); break;

		case SYS_uname:     result = sc_uname ((void*)(argptr[1]));                                break;

		case SYS_creat:     result = sc_creat ((char*)(argptr[1]), argptr[2]);                     break; // May be obsolete

		case SYS_getcwd:    result = sc_getcwd((char*)(argptr[1]), argptr[2]);                     break;

		case SYS_execve:    result = sc_execve((char*)(argptr[1]), (char**)(argptr[2]), (char**)(argptr[3])); break;

		case SYS_T_alloc:   result = sc_t_alloc  ((size_t)(argptr[1])); break;
		case SYS_T_env_get: result = sc_t_env_get((char* )(argptr[1])); break;
		case SYS_T_env_set: result = sc_t_env_set((char* )(argptr[1]), (char*)(argptr[2])); break;
                case SYS_T_screen_apply: result = sc_t_screen_apply
                                                         ((surface*)(argptr[1]), *(rect*)(argptr + 2)); break;
		default: //panic("Invalid syscall");
			printf("Invalid syscall #%i\n", _eax);
			asm("cli;hlt;");
	}

	_eax = result;
	asm volatile("sti;" : : "a"(_eax), "b"(_ebx), "c"(_ecx), "d"(_edx));
}
