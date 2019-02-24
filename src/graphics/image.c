#include <modules.h>
#ifdef MODULE_IMG
//Based on stb_image.
#include <graphics/surface.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO


#include <stb_image.h>
#include <stdio.h>

//Small and simple function that acts like "glue" between stb_image and Surface.
//It loads images ONLY from memory, because our VFS is very bad.
surface *surface_load_image(unsigned char *buf, int length)
{
	int w, h, components;
	unsigned char *bitmap = stbi_load_from_memory(buf, length, &w, &h, &components, 0 /* even if image is monochrome we need it to be RGBA! */);
	surface *sf=surface_new();
	surface_default_init(sf, w, h);
	//Our buffer contains w*h 32-bit RGBA pixels, so now we will fill the surface with them.
	for(int y=0; y<h; y++)
		for(int x=0; x<w; x++, bitmap+=components)
		{
			switch(components)
			{
				case STBI_grey:
				{
					color_rgba color = {bitmap[0], bitmap[0], bitmap[0], 0xFF};
					surface_set_pixel(sf, color, x, y);
					break;
				}
				case STBI_grey_alpha:
				{
					color_rgba color2 = {bitmap[0], bitmap[0], bitmap[0], bitmap[1]};
					surface_set_pixel(sf, color2, x, y);
					break;
				}
				case STBI_rgb:
				{
					color_rgba color3 = {bitmap[0], bitmap[1], bitmap[2], 0xFF};
					surface_set_pixel(sf, color3, x, y);
					break;
				}
				case STBI_rgb_alpha:
				{
					color_rgba color4 = {bitmap[0], bitmap[1], bitmap[2], bitmap[3]};
					surface_set_pixel(sf, color4, x, y);
					break;
				}
			}
		}
	return sf;
}

#endif
