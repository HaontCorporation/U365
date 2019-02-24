//TrueType font drawing using stb_truetype.
//Define STB_TRUETYPE_IMPLEMENTATION, so sources will be generated there.
#define MODULE_TTF
#ifdef MODULE_TTF

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <memory.h>
#include <truetype.h>
#include <graphics/surface.h>
#include <utf8.h>
#include <string.h>
#include <stdio.h>

//Now we have full TrueType code. Header functions won't be used in apps or something, we will create API that works as layer between Surface and TrueType.
surface *draw_ttf_char(unsigned char *font, int character, int size, color_rgba color)
{
	stbtt_fontinfo font1;
	unsigned char *bitmap;
	int w,h,i,j,c=character;
	stbtt_InitFont(&font1, font, stbtt_GetFontOffsetForIndex(font,0));
	bitmap = stbtt_GetCodepointBitmap(&font1, 0,stbtt_ScaleForPixelHeight(&font1, size), c, &w, &h, 0,0);
	surface *sf=surface_new();
	//It's not a full bitmap, because only ~1/3 of it is character box.
	surface_default_init(sf, w, h);
	for(j=0; j<h; j++)
	{
		for(i=0; i<w; i++)
		{
			color_rgba co = {color.r, color.g, color.b, bitmap[j * w + i]};
			surface_set_pixel(sf, co, i, j);
		}
	}
	return sf;/*
	stbtt_bakedchar *s = malloc(sizeof(stbtt_bakedchar)); //This contains character box, so our font won't be monospace like VGA fonts.
	unsigned char* buf = malloc(size*size);
	stbtt_BakeFontBitmap(font, 0, size, buf, size, size, character, 1, s);
	//Now we will create surface.
	surface *sf=surface_new();
	//It's not a full bitmap, because only ~1/3 of it is character box.
	surface_default_init(sf, size, size);
	for(int j=0; j<size; j++)
	{
		for(int i=0; i<s->x1; i++)
		{
			color_rgba c = {color.r, color.g, color.b, buf[j * size + i]};
			surface_set_pixel(sf, c, i, j);
		}
	}
	//Dirty hack that "crops" image (no image data is deleted, only size changed).
	sf->w = s->x1;
	return sf;*/
}
surface *draw_ttf_string(unsigned char *font, const char *utf8_str, int size, color_rgba color)
{
	stbtt_fontinfo font1;
	uint32_t *str = calloc(strlen(utf8_str) + 1, 4);
	utf8_convert(utf8_str, str);
	stbtt_InitFont(&font1, font, stbtt_GetFontOffsetForIndex(font,0));
	unsigned char *char_bitmaps[utf8_strlen(str)];
	int width_arr[utf8_strlen(str)], height_arr[utf8_strlen(str)];
	int w, h, i, j, curr_x=0;
	for(unsigned i=0; i<utf8_strlen(str); i++)
	{
		char_bitmaps[i] = stbtt_GetCodepointBitmap(&font1, 0,stbtt_ScaleForPixelHeight(&font1, size), str[i], &w, &h, 0,0);
		curr_x+=w+4;
		switch(str[i])
		{
			case ' ' : width_arr[i] = size/4; break;
			case '\t': width_arr[i] = size/2; break;
			default: width_arr [i] = w; break;
		}
		height_arr[i] = h;
	}
	surface *sf=surface_new();
	//It's not a full bitmap, because only ~1/3 of it is character box.
	surface_default_init(sf, curr_x, size);
	curr_x=0; //It will be reused when font will be drawn
	for(unsigned k=0; k<utf8_strlen(str); k++)
	{
		for(j=0; j<height_arr[k]; j++)
		{
			for(i=0; i<width_arr[k]; i++)
			{
				color_rgba co = {color.r, color.g, color.b, char_bitmaps[k][j * width_arr[k] + i]};
				surface_set_pixel(sf, co, curr_x+i+k*2, sf->h-height_arr[k]+j);
			}
		}
		curr_x+=width_arr[k];
	}
	return sf;
}
//Dirty hack to make ttf_write_text work.
//Basically this has no changes but removal of decoding from UTF-8 because it was done already in ttf_write_text.
surface *draw_ttf_text_wrstring(unsigned char *font, const uint32_t *str, int size, color_rgba color)
{
	stbtt_fontinfo font1;
	stbtt_InitFont(&font1, font, stbtt_GetFontOffsetForIndex(font,0));
	unsigned char *char_bitmaps[utf8_strlen(str)];
	int width_arr[utf8_strlen(str)], height_arr[utf8_strlen(str)];
	int w, h, i, j, curr_x=0;
	for(unsigned i=0; i<utf8_strlen(str); i++)
	{
		char_bitmaps[i] = stbtt_GetCodepointBitmap(&font1, 0,stbtt_ScaleForPixelHeight(&font1, size), str[i], &w, &h, 0,0);
		curr_x+=w+4;
		switch(str[i])
		{
			case ' ' : width_arr[i] = size/4; break;
			case '\t': width_arr[i] = size/2; break;
			default: width_arr [i] = w; break;
		}
		height_arr[i] = h;
	}
	surface *sf=surface_new();
	//It's not a full bitmap, because only ~1/3 of it is character box.
	surface_default_init(sf, curr_x, size);
	curr_x=0; //It will be reused when font will be drawn
	for(unsigned k=0; k<utf8_strlen(str); k++)
	{
		for(j=0; j<height_arr[k]; j++)
		{
			for(i=0; i<width_arr[k]; i++)
			{
				color_rgba co = {color.r, color.g, color.b, char_bitmaps[k][j * width_arr[k] + i]};
				surface_set_pixel(sf, co, curr_x+i+k*2, sf->h-height_arr[k]+j);
			}
		}
		curr_x+=width_arr[k];
	}
	return sf;
}
/*
	ttf_write_text(): Return surface with written text.
	Has support for newlines and tabulations.
	Has support for text align: left-/center-/right-sided text can be rendered by setting direction parameter.
*/
surface *ttf_write_text(unsigned char *font, const char* utf8_str, int size, color_rgba color, enum ttf_text_direction direction)
{
	//Text is broken by lines, separator is '\n'. Also we don't have support for user-provided surfaces to prevent incorrect output when text width/height > surface's width/height.
	unsigned lines = 0;
	const uint32_t **line_array = malloc(surface_screen->h/size * sizeof(const char *));
	uint32_t *str = calloc(strlen(utf8_str) + 1, 4);
	utf8_convert(utf8_str, str);
	for(unsigned i=0; str[i] != '\0'; i++)
	{
		if(str[i] == '\n')
		{
			//i++; //skip newline ;)
			line_array[lines] = &str[i];
			unsigned line_length; //before '\n'!
			for(line_length = 0; str[i+line_length] != '\n'; line_length++)
				;
			//str[i+line_length] = '\0';
			lines++;
		}
	}
	printf("%d\n", lines);
	surface **line_surfaces = malloc(lines * sizeof(surface *));
	for(unsigned i=0; i < lines; i++)
	{
		line_surfaces[i] = draw_ttf_text_wrstring(font, line_array[i]-0x10, size, color);
	}
	surface *ret = surface_new(); //This is the actual surface with text.
	uint32_t most_long_line_width = 0;
	uint32_t current_longest_line = line_surfaces[0]->w;
	for(unsigned i=0; i<lines; i++)
		if(line_surfaces[i]->w > current_longest_line)
			current_longest_line = line_surfaces[i]->w;
	most_long_line_width = current_longest_line;
	//We did this to create surface that fits all lines.
	surface_default_init(ret, most_long_line_width, size * lines);
	printf("%u\n", most_long_line_width);
	for(unsigned current_line = 0; current_line < lines; current_line++)
	{
		//Actually apply text to the surface.
		uint32_t x = 0;
		switch(direction)
		{
			case DIRECTION_LEFT: x = 0; break; //Start from left side
			case DIRECTION_CENTER: x = ret->w / 2 - line_surfaces[current_line]->w / 2; break; //Yes, little more complex than left or right.
			case DIRECTION_RIGHT: x = ret->w - line_surfaces[current_line]->w; break;
		}
		rect r = {x, current_line * size, line_surfaces[current_line]->w, line_surfaces[current_line]->h};
		surface_apply(ret, line_surfaces[current_line], r);
	}
	return ret;
}

#endif // MODULE_TTF
