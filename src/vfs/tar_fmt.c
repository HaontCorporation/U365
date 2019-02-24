//tar format support in kernel, used for initrd extracting
#include <tar.h>
#include <debug.h>
#include <stdbool.h>
#include <stdint.h>

unsigned int tar_getsize(const char *in) //Copied from OSDev.org, so don't say that I've stolen it.
{
	unsigned int size = 0;
	unsigned int j;
	unsigned int count = 1;

	for (j = 11; j > 0; j--, count *= 8)
		size += ((in[j - 1] - '0') * count);

	return size;
}

unsigned int tar_gethdrcnt(const void* tar)
{
	const tar_hdr_t *hdr = (const tar_hdr_t*) tar;
	int i = 0;
	for(i = 0; hdr->name[0]!='\0'; i++)
	{
		tar+=((tar_getsize(hdr->size) / sizeof(tar_hdr_t)) + 1) * sizeof(tar_hdr_t);
		if (tar_getsize(hdr->size) % 512)
			tar += 512;
		hdr = (const tar_hdr_t*) tar;
	}
	return i;
}
