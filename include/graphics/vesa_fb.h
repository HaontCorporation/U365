#ifndef __VESA_FB_H
#define __VESA_FB_H

#include <stdint.h>
#include "arch/i686/mboot.h"

#ifdef __cplusplus
extern "C" {
#endif

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

extern unsigned int vesa_fb_width, vesa_fb_height, vesa_fb_bpp, vesa_fb_pitch;
extern uint8_t     *vesa_fb_addr;
void initVBE(struct mboot_info*);
void putPix(unsigned int, unsigned int, uint32_t);
uint32_t getPix(int,int);

#ifdef __cplusplus
}
#endif

#endif
