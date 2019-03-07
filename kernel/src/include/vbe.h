#ifndef __VESA_FB_H
#define __VESA_FB_H

#include <stdint.h>

enum colors // DEPRECATED
{
    BLACK = 0x000000,
    BLUE = 0x0000AA,
    GREEN = 0x00AA00,
    CYAN = 0x00AAAA,
    RED = 0xAA0000,
    MAGENTA = 0xAA00AA,
    BROWN = 0xAA5500,
    LIGHT_GREY = 0xAAAAAA,
    DARK_GREY = 0x555555,
    LIGHT_BLUE = 0x5555FF,
    LIGHT_GREEN = 0x55FF55,
    LIGHT_CYAN = 0x55FFFF,
    LIGHT_RED = 0xFF5555,
    LIGHT_MAGENTA = 0xFF55FF,
    LIGHT_BROWN = 0xffff55,
    WHITE = 0xFFFFFF,
};

typedef struct __vbe_modeinfo
{
    uint16_t attributes;	
	uint8_t window_a;			
	uint8_t window_b;			
	uint16_t granularity;		
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;	
	uint16_t pitch;			
	uint16_t width;			
	uint16_t height;			
	uint8_t w_char;			
	uint8_t y_char;			
	uint8_t planes;
	uint8_t bpp;
	uint8_t banks;			
	uint8_t memory_model;
	uint8_t bank_size;		
	uint8_t image_pages;
	uint8_t reserved0;
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
	uint32_t framebuffer;		
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;
	uint8_t reserved1[206];
} __attribute__((packed)) vbe_modeinfo; 

extern unsigned int vesa_fb_width, vesa_fb_height, vesa_fb_bpp, vesa_fb_pitch;
extern uint8_t     *vesa_fb_addr;
void initVBE();
void putPix(unsigned int, unsigned int, uint32_t);
uint32_t getPix(int,int);

#ifdef __cplusplus
}
#endif

#endif