#include <stdint.h>

#include "arch/i686/mboot.h"
#include "arch/i686/devices.h"

#include "graphics/vesa_fb.h"
#include "graphics/surface.h"

//VBE Linear frame buffer small driver.

unsigned int vesa_fb_width, vesa_fb_height, vesa_fb_bpp, vesa_fb_pitch;
uint8_t     *vesa_fb_addr;

void putPix(unsigned int x, unsigned int y, uint32_t color)
{
    if(kernel_mboot_info->framebuffer_type!=1)
        return;
    color_rgba col;
    col.r = color >> 16;
    col.g = color >> 8;
    col.b = color;
    col.a = 0xFF;
    if(surface_screen)
    {
        surface_set_pixel(surface_screen, col, x, y);
    }
    else
    {
        if(x>=vesa_fb_width || y>=vesa_fb_height)
            return;
        unsigned where = x*(vesa_fb_bpp/8) + y*vesa_fb_pitch;
        vesa_fb_addr[where + 0] = col.b;
        vesa_fb_addr[where + 1] = col.g;
        vesa_fb_addr[where + 2] = col.r;
    }
}
/*
Initializing VESA mode. You'll need a Multiboot struct to work, because it's using info from it.
*/
void initVBE(struct mboot_info *mboot)
{
    //First, get the LFB address from MB info struct.
    vesa_fb_addr = (uint8_t*)(int)(mboot->framebuffer_addr);
    //Then get the pitch.
    vesa_fb_pitch=mboot->framebuffer_pitch;
    //Then BPP.
    vesa_fb_bpp=mboot->framebuffer_bpp;
    //And, finally, screen width and height.
    vesa_fb_width = mboot->framebuffer_width  ;
    vesa_fb_height = mboot->framebuffer_height;

    // TODO: insert into devtable
}
