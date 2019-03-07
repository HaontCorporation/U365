#include <stdint.h>
#include <real.h>
#include <stdio.h>
#include <io.h>
#include <vbe.h>
#include <surface.h>
#include <string.h>

extern void *vbe_init[];
extern void *vgafnt_dump[];
extern vbe_modeinfo vbe_modeinfo_data[];

//VBE Linear frame buffer small driver.

unsigned int vesa_fb_width, vesa_fb_height, vesa_fb_bpp, vesa_fb_pitch;
uint8_t     *vesa_fb_addr;

void putPix(unsigned int x, unsigned int y, uint32_t color)
{
    //if(kernel_mboot_info->framebuffer_type!=1)
    //    return;
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
      vesa_fb_addr[where + 0] = col.r;
      vesa_fb_addr[where + 1] = col.g;
      vesa_fb_addr[where + 2] = col.b;
    }
}
/*
Initializing VESA mode. You'll need a Multiboot struct to work, because it's using info from it.
*/

void tmp(FILE* file, int character)
{
    outb(0xE9, character);
}

void tmp2(FILE* file, const char *string)
{
    outsb(0xE9, string, strlen(string));
}

void initVBE()
{
    exec_real(vbe_init);
    exec_real(vgafnt_dump);
    //First, get the LFB address from the VBE ModeInfo Structure.
    vesa_fb_addr = vbe_modeinfo_data->framebuffer;
    //Then get the pitch.
    vesa_fb_pitch = vbe_modeinfo_data->pitch;
    //Then BPP.
    vesa_fb_bpp = vbe_modeinfo_data->bpp;
    //And, finally, screen width and height.
    vesa_fb_width = vbe_modeinfo_data->width;
    vesa_fb_height = vbe_modeinfo_data->height;

    // TODO: insert into devtable
}