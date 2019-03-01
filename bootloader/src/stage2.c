
#include <stdint.h>
#include <stddef.h>

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;

typedef struct {
    unsigned char   e_ident[16];
    Elf64_Half      e_type;
    Elf64_Half      e_machine;
    Elf64_Word      e_version;
    Elf64_Addr      e_entry;
    Elf64_Off       e_phoff;
    Elf64_Off       e_shoff;
    Elf64_Word      e_flags;
    Elf64_Half      e_ehsize;
    Elf64_Half      e_phentsize;
    Elf64_Half      e_phnum;
    Elf64_Half      e_shentsize;
    Elf64_Half      e_shnum;
    Elf64_Half      e_shstrndx;
} Elf64_Ehdr;

typedef struct {
	Elf64_Word	p_type;
	Elf64_Word	p_flags;
	Elf64_Off	p_offset;
	Elf64_Addr	p_vaddr;
	Elf64_Addr	p_paddr;
	Elf64_Xword	p_filesz;
	Elf64_Xword	p_memsz;
	Elf64_Xword	p_align;
} Elf64_Phdr;

typedef struct {
	Elf64_Word	sh_name;
	Elf64_Word	sh_type;
	Elf64_Xword	sh_flags;
	Elf64_Addr	sh_addr;
	Elf64_Off	sh_offset;
	Elf64_Xword	sh_size;
	Elf64_Word	sh_link;
	Elf64_Word	sh_info;
	Elf64_Xword	sh_addralign;
	Elf64_Xword	sh_entsize;
} Elf64_Shdr;

void *memset(void *dest, int value, size_t count)
{
    for (int i = 0; i < count; i++)
    {
        ((uint8_t *)dest)[i] = value;
    }
}

void *memcpy(void *dest, void *src, size_t count)
{
    for (int i = 0; i < count; i++)
    {
        ((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
    }
}

uint64_t entry;

Elf64_Addr load_elf(uint32_t ElfAddress)
{
	Elf64_Ehdr *ehdr = ElfAddress;
	Elf64_Phdr *phdr = ElfAddress + ehdr->e_phoff;
	Elf64_Shdr *shdr = ElfAddress + ehdr->e_shoff;
	
	for (int i = 0; i < ehdr->e_phnum; i++)
	{
		if (phdr[i].p_type == 1)
		{
			memset(phdr[i].p_paddr, 0, phdr[i].p_memsz);
			memcpy(phdr[i].p_paddr, ElfAddress + phdr[i].p_offset, phdr[i].p_filesz);
		}
	}

	for (int i = 0; i < ehdr->e_shnum; i++)
	{
		if (shdr[i].sh_type == 0x08)
			memset(shdr[i].sh_addr, 0, shdr[i].sh_size);
	}

    entry = ehdr->e_entry;
}