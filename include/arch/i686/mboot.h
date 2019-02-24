#ifndef __MBOOT_H
#define __MBOOT_H

#include <stdint.h>
/**
 * VERY important.
 * For those who say that that OS isn't written from scratch.
 * THAT STRUCTURES ARE COPIED FROM GLAUXOS. NOT STOLEN. ITS
 * DEVELOPER ALLOWED ME TO USE THEM.
 */

typedef struct mboot_info mboot_info;

struct mboot_info {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;

	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;

	uint64_t framebuffer_addr;
	uint32_t framebuffer_pitch;
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;
	uint8_t framebuffer_bpp;
	uint8_t framebuffer_type;
	union
	{
		struct
		{
			uint32_t framebuffer_palette_addr;
			uint16_t framebuffer_palette_num_colors;
		};
		struct
		{
			uint8_t framebuffer_red_field_position;
			uint8_t framebuffer_red_mask_size;
			uint8_t framebuffer_green_field_position;
			uint8_t framebuffer_green_mask_size;
			uint8_t framebuffer_blue_field_position;
			uint8_t framebuffer_blue_mask_size;
		};
	};
};

struct mboot_mod_list_struct
{
	uint32_t mod_start;
	uint32_t mod_end;
	uint32_t cmdline;
	uint32_t pad;
};

struct mboot_mmap
{
	unsigned int size : 4 ;
	uint8_t      base_addr;
	uint8_t      length   ;
	uint8_t      type     ;
};

void parse_memlist(mboot_info* mboot);
/*
extern void*    memlist;
extern uint64_t memlist_size;*/

extern struct mboot_info *kernel_mboot_info;
#endif
