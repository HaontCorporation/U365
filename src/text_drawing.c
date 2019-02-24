#include <graphics/surface.h>
#include <vgafnt.h>
#include <stdint.h>
#include <memory.h>
#include <stddef.h>
#include <text_drawing.h>
//To speed up the code we use font bitmap caching.
static uint8_t ***vga_font_character_cache;
/* It's separated from terminal code, because we need to use terminal before Surface
 * initialization too. */
void draw_vgafnt_character(surface *obj, char c, int x, int y,
								 color_rgba fg, color_rgba bg)
{
	if(vga_font_character_cache[(int)c] != NULL)
	{
		for(int cy=0; cy<VGAFNT_CHARACTER_HEIGHT; cy++)
		{
			for(int cx=0; cx<VGAFNT_CHARACTER_WIDTH; cx++)
				if(vga_font_character_cache[(int)c][cy][cx] != 0)
					surface_set_pixel(obj, fg, x+cx, y+cy);
				else
					surface_set_pixel(obj, bg, x+cx, y+cy);
		}
		return;
	}
	//Code isn't mine.
	vga_font_character_cache[(int)c] = malloc(VGAFNT_CHARACTER_WIDTH * VGAFNT_CHARACTER_HEIGHT);
    int cx,cy;
    int mask[8]={128,64,32,16,8,4,2,1};
    unsigned char *glyph=(uint8_t*)vgafnt+(int)c*VGAFNT_CHARACTER_HEIGHT;
    for(cy=0;cy<16;cy++){
        for(cx=0;cx<8;cx++){
            if(glyph[cy]&mask[cx])
            {
            	vga_font_character_cache[(int)c][cy][cx] = 1;
            	surface_set_pixel(obj, fg, x+cx, y+cy);
            }
            else
            {
            	vga_font_character_cache[(int)c][cy][cx] = 0;
            	surface_set_pixel(obj, bg, x+cx, y+cy);
            }
        }
    }
}
void write_vgafnt_string  (surface *obj, char *str, int x, int y,
									color_rgba fg, color_rgba bg)
{
	for(int i=0; str[i]!='\0'; i++)
		draw_vgafnt_character(obj, str[i], x+i*VGAFNT_CHARACTER_WIDTH, y, fg, bg);
}
void null_vgafnt_character_cache()
{
	for(int i=0; i<256; i++) //We have exactly 256 glyphs in font.
	{
		//free(vga_font_character_cache[i]);
		vga_font_character_cache[i]=NULL;
	}
}