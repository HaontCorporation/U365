#include <elf_hdr.h>
#include <stdio.h>
#include <string.h>
#include <fio.h>
#include <debug.h>
#include <fs.h>
#include <arch/i686/mboot.h>
#include <time.h>

#include <proctab.h>

unsigned int get_file_size(const char *name)
{
	FILE* fp=fopen(name, "r");
	if(fp == NULL) return 0;
	unsigned int i;
	int16_t c=fgetc(fp);
	for(i=0; c!=EOF; i++)
		c=fgetc(fp);
	return i;
}

//Returns 0 if header is valid, 1 if magic number invalid, 2 and more if file isn't compatible with U365.
uint8_t elf_check_header(struct elf_hdr* hdr)
{
	if(hdr->mag_num[0]!=0x7f || hdr->mag_num[1]!='E' || hdr->mag_num[2]!='L' || hdr->mag_num[3]!='F')
		return 1; //our magic number isn't valid.
	if(hdr->arch!=ELF_ARCH_32BIT)
		return 2; //This ELF file can't be loaded by U365 because it's not 32-bit.
	if(hdr->byte_order!=ELF_BYTEORDER_LENDIAN)
		return 3; //This ELF file can't be loaded by U365 because it's not little-endian.
	if(hdr->elf_ver!=1)
		return 4; //This ELF file can't be loaded by U365 because its version isn't 1.
	if(hdr->file_type!=ELF_REL && hdr->file_type!=ELF_EXEC)
		return 5; //This ELF file can't be loaded by U365 because it's neither executable nor relocatable file.
	if(hdr->machine!=ELF_386_MACHINE)
		return 6; //This ELF file can't be loaded by U365 because it's not for i386 platform.
	return 0;

}
void *elf_open(const char* name) //Returns pointer to ELF file.
{
	struct stat stat_info;
	FILE* fp = fopen(name, "r");
	if(stat(name, &stat_info) || !fp)
	{
		return 0;
	}
	void* addr;
	addr = malloc(stat_info.st_size + 1);
	int16_t tread;
	for(int i = 0; (tread = fgetc(fp)) != EOF; i++)
		((uint8_t*)(addr))[i] = tread;
	struct elf_hdr *hdr = addr;
	uint8_t status = elf_check_header(hdr);
	if(status)
	{
		return 0;
	}
	else return hdr;
}
struct elf_section_header *elf_get_section_header(void *elf_file, int num)
{
	struct elf_hdr *hdr=(struct elf_hdr*) elf_file;
	return (struct elf_section_header *)(elf_file+hdr->shoff+hdr->sh_ent_size*num);
}
struct elf_program_header *elf_get_program_header(void *elf_file, int num)
{
	struct elf_hdr *hdr=(struct elf_hdr*) elf_file;
	return (struct elf_program_header *)(elf_file+hdr->phoff+hdr->ph_ent_size*num);
}
const char *elf_get_section_name(void *elf_file, int num)
{
	struct elf_hdr *hdr=(struct elf_hdr*) elf_file;
	return (hdr->sh_name_index==SH_UNDEF) ? "no section" : (const char*)elf_file+elf_get_section_header(elf_file, hdr->sh_name_index)->offset+elf_get_section_header(elf_file, num)->name;
}
void elf_hdr_info(struct elf_hdr *hdr)
{
	printf("\tHeader information:\n");
	printf("\t\tArchitecture: %s\n", (hdr->arch==ELF_ARCH_32BIT) ? "32-bit" : (hdr->arch==ELF_ARCH_64BIT) ? "64-bit" : "Unknown architecture");
	printf("\t\tByte order: %s\n", (hdr->byte_order==ELF_BYTEORDER_LENDIAN) ? "little-endian" : "unknown");
	printf("\t\tELF file version: %u\n", hdr->elf_ver);
	printf("\t\tELF file type: %s\n", (hdr->file_type==ELF_REL) ? "relocatable" : (hdr->file_type==ELF_EXEC) ? "executable" : "unknown");
	printf("\t\tTarget machine: %s\n", (hdr->machine==ELF_386_MACHINE) ? "i386" : "unknown");
	printf("\t\tEntry point: 0x%08X\n", hdr->entry);
	printf("\t\tProgram header offset: %u\n", hdr->phoff);
	printf("\t\tSection header offset: %u\n", hdr->shoff);
	printf("\t\tFile flags: %u\n", hdr->flags);
	printf("\t\tFile header size: %u\n", hdr->hsize);
	printf("\t\tProgram header entry size: %u\n", hdr->ph_ent_size);
	printf("\t\tSection header entry size: %u\n", hdr->sh_ent_size);
	printf("\t\tSection header count: %d\n", hdr->sh_ent_cnt);
	printf("\t\tProgram header count: %d\n", hdr->ph_ent_cnt);
}

// MOVE IT INTO TTY AS SOON AS POSSIBLE
struct kbdbuf
{
	char buf[1025];
	int size;
	int pos;
} kbuf;

int fldes_tty_getc(file_interface* UNUSED(iface))
{
	while(kbuf.pos >= kbuf.size)
	{
		kbuf.size = kbd_gets(kbuf.buf, 1024);
		kbuf.pos = 0;
	}
	return kbuf.buf[kbuf.pos++];
}

int fldes_tty_putc(file_interface* UNUSED(iface), char c)
{
	return putchar(c);
}

uint32_t lastbin_ep;
int run_elf_file(const char* name, char **argv, char** env __attribute__((unused)), int argc /* DIRTY HACK, REMOVE IT ASA */) //Execute ELF file. There's no way to get back at this time, but you can use syscalls to access U365 kernel's functions.
{
    if(!name || !argv || !env)
        return -1;
    char** final_argv = malloc(sizeof(char**) * argc);
    final_argv[0] = (char*) name;
    for(int i=1; i<argc; i++)
    {
        final_argv[i] = argv[i-1];
    }
    void *elf_file=elf_open(name); //elf_open tries to open ELF file and returns pointer to file data on success, else 0.
	if(elf_file==NULL)
		return -1; //It's not valid ELF file!

	struct elf_hdr *hdr=(struct elf_hdr *)elf_file;
	//First we need to put file segments at valid addresses.

	for(int i=0; i<hdr->ph_ent_cnt; i++)
	{
		//printf("Segment [%i/%i]: ", i, hdr->ph_ent_cnt);
		struct elf_program_header *phdr=elf_get_program_header(elf_file, i);
		if(phdr->type!=SEGTYPE_LOAD)
		{
			//printf("DAT SHIT IS NOT LOAD\n");
			continue; //We only can load segments to the memory, so just skip it.
		}
		//printf("Loading 0x%08x bytes to 0x%08x\n", phdr->size_in_mem, phdr->load_to);
		memset((void*)phdr->load_to, 0, phdr->size_in_mem); //Null segment memory.
		memcpy((void*)phdr->load_to, elf_file+phdr->data_offset, phdr->size_in_file);
	}

	for(int i=0; i<hdr->sh_ent_cnt; i++)
	{
		struct elf_section_header *shdr = (struct elf_section_header *)((uint8_t*)(elf_file) + hdr->shoff + hdr->sh_ent_size * i);

		if(shdr->addr)
		{
			memcpy((void*)shdr->addr, elf_file+shdr->offset, shdr->size > 0x1000 ? 0x1000 : shdr->size);
			if(!strcmp(elf_get_section_name(elf_file, i), ".bss"))
			{
				;
			}
		}
	}
	//Now we've done everything. Let's run the ELF file!
#ifdef MODULE_PROCTAB
	// Insert the process into proctab
	proctab_entry* ent = calloc(sizeof(proctab_entry), 1);
	ins_proc(ent);
	pid_t tid = ent->pid;
	c_pid = tid;
    printk("New PID: %i\n", tid);

	// Prepare file descriptors
	ent->files = vector_new();
	vector_resize(ent->files, 3);

	file_interface* fldes_stdin  = calloc(sizeof(file_interface), 3);
	file_interface* fldes_stdout = fldes_stdin + 1;
	file_interface* fldes_stderr = fldes_stdin + 2;
	fldes_stdin ->getc   = fldes_tty_getc;
	fldes_stdout->putc   = fldes_tty_putc;
	fldes_stderr->putc   = fldes_tty_putc;
	fldes_stdin ->opened = 1;
	fldes_stdout->opened = 1;
	fldes_stderr->opened = 1;
	*vector_at(ent->files, 0) = fldes_stdin;
	*vector_at(ent->files, 1) = fldes_stdout;
	*vector_at(ent->files, 2) = fldes_stderr;

	ent->name = "[elf]";
#endif

    int(*entry_point)(int, char**) = (void*)(hdr->entry);
	//printf("ELF entry point: 0x%08x\n", hdr->entry);
    int result = entry_point(argc, final_argv);
    printf("[%s] Return value was %i\n", name, result);

#ifdef MODULE_PROCTAB
	rm_proc(find_proc(tid));
#endif

	return 0; //It has been an one-way ticket, so this will never execute...
}
void elf_info_short(const char* name)
{
	void *elf_file=elf_open(name);
	if(elf_file==NULL)
		return;
	struct elf_hdr *hdr=(struct elf_hdr *)elf_file;
	printf("\tName: %s\n", name);
	printf("\tFile size: %u\n\tELF file info:\n", get_file_size(name));
	elf_hdr_info(hdr);
	printf("\t\tSection list:\n");
	for(int i=1; i<hdr->sh_ent_cnt; i++)
	{
		struct elf_section_header *shdr=(struct elf_section_header *)(elf_file+hdr->shoff+hdr->sh_ent_size*i);
		printf("\t\t\tSection %d: name: %s, data offset: %u.\n", i, elf_get_section_name(elf_file, i), shdr->offset, shdr->name, i);
	}
}
void elf_info(const char* name)
{
	void *elf_file=elf_open(name);
	if(elf_file==NULL)
		return;
	struct elf_hdr *hdr=(struct elf_hdr *)elf_file;
	printf("\tName: %s\n", name);
	//memcpy((void*)hdr->entry, elf_file, get_file_size(name));
	/*asm("\
				.intel_syntax noprefix;\
				mov eax, 0;\
				mov ebx, '^';\
				int 0x80;\
				.att_syntax prefix");*/
	printf("\tFile size: %u\n\tELF file info:\n", get_file_size(name));
	elf_hdr_info(hdr);
	//tty_wrstr("\n\e[32m6\n\e[32md\n\e[32me\n\e[32md\n\e[32ma\n\e[32md\n\e[32mf\n\e[92m8\n\e[92mB\n\e[92m6\n\e[92ma\n\e[36mA\n\e[96mB\n\e[0m");
	for(int i=0; i<hdr->sh_ent_cnt; i++)
	{
		struct elf_section_header *shdr=(struct elf_section_header *)(elf_file+hdr->shoff+hdr->sh_ent_size*i);
		printf("\t\t\tSection %d:\n", i);
		printf("\t\t\t\tActual section offset: %u\n\t\t\t\tSection name offset in string table: %d\n", shdr->offset, shdr->name);
		printf("\t\t\t\tSection name: %s\n", elf_get_section_name(elf_file, i));
	}
}
