#include <stdio.h>
#include <graphics/surface.h>
#include <memory.h>
#include <truetype.h>
#include <ttf.h>
#include <env.h>
#include <time.h>
#include <modules.h>
//#include <arch/i686/devices.h>

int font_viewer()
{
	int chrcode = 0;
	uint8_t *font = ttf;
	printf("Use arrow keys to move between the characters and Q to exit. Let's start!\n");
	unsigned char key = 0;
	uint8_t to_redraw = 1;
	while((key = getchar()) != 'q') //This will exit the application.
	{
		switch(key)
		{
			case 128+12+10: if(chrcode>0)   { chrcode--; to_redraw = 1; } break;
			case 128+12+11: if(chrcode<256) { chrcode++; to_redraw = 1; } break;
		}
		if(!to_redraw)
			continue;
		printf("\rCurrent character: '%c' (code %u)\t\t", (chrcode == '\n') ? 0 : chrcode, chrcode);
		rect border_rect = {650, 50, 100, 100};
		rect fill_rect   = {651, 51, 98, 98};
		surface_fill_rect(surface_screen, color_gray, border_rect);
		surface_fill_rect(surface_screen, color_white, fill_rect);
		surface *sf = draw_ttf_char(font, chrcode, 40, color_black);
		rect applyRect = {650+100 / 2 - sf->w/2, 50+100 / 2 - sf->h/2, sf->w, sf->h};
		surface_apply(surface_screen, sf, applyRect);
		to_redraw = 0;
	}
	return 0;
}
int cmd_write_ttf_str()
{
	uint8_t *font = ttf;
	printf("Enter your text: ");
	char* input = malloc(128);
	kbd_gets(input, 127);
	surface* text = draw_ttf_string(font, input, 16, color_black);
	//We will fill a small area under the text with some color so it definitely will be readable.
	rect background_area = {10, 10, text->w+20, text->h+20};
	surface_fill_rect(surface_screen, color_light_gray, background_area);
	rect text_area = {20, 20, text->w, text->h};
	surface_apply(surface_screen, text, text_area);
	return 0;
}
//This function is not actually a test, but a test polygon for my maybe stupid dreams.
void surface_test(int argc, char *argv[])
{
	if(argc<2)
	{
		fprintf(stderr, "error: no arguments. Usage:\n"
						"\tdesktop\talmost full Surface test\n"
						"\tblur\tTest of my stupid blurring algorithm.\n"
						"\tmask\tSurface masking test.\n");
		return;
	}
	if(strcmp(argv[1], "desktop")==0)
	{
		printf("Selected '%s' test.\n", argv[1]);
		rect background_rect = {0,0,surface_screen->w, surface_screen->h};
		rect panel_rect = {0, 0, surface_screen->w, 25};
		rect wnd_inner_rect = {200, 200, 700, 400};
		rect wnd_caption_rect = {200, 200, 700, 40};
		rect wnd_shadow_rect = {202, 202, 702, 402};
		rect notif_rect = {512, 50, 1000, 125};
		rect switch_rect = {210, 250, 20, 10};
		rect switch_on_rect = {210, 250, 10, 10};
		color_rgba background_color ={0x81, 0xD4, 0xFA, 0xFF};
		color_rgba panel_color ={0xDD, 0xDD, 0xDD, 200};
		color_rgba notif_color ={0xFF, 0xCC, 0x00, 0x77};
		color_rgba wnd_inner_color ={0xEE, 0xEE, 0xEE, 0xFF};
		color_rgba wnd_caption_color ={0x75, 0x75, 0x75, 0xFF};
		color_rgba wnd_shadow_color ={0x44, 0x44, 0x44, 0x55};
		color_rgba switch_color ={0xAA, 0xAA, 0xAA, 0xFF};
		color_rgba switch_on_color ={0x76, 0xFF, 0x03, 0xA0};
		printf("Drawing started.\n");
		surface* buf=surface_new();
		surface_default_init(buf, surface_screen->w, surface_screen->h);
		surface_fill_rect(buf, background_color, background_rect);
		surface_fill_rect(buf, panel_color, panel_rect);
		surface_fill_rect(buf, notif_color, notif_rect);
		surface_fill_rect(buf, wnd_shadow_color, wnd_shadow_rect);
		surface_fill_rect(buf, wnd_inner_color, wnd_inner_rect);
		surface_fill_rect(buf, wnd_caption_color, wnd_caption_rect);
		surface_fill_rect(buf, switch_color, switch_rect);
		surface_fill_rect(buf, switch_on_color, switch_on_rect);
		surface_fill_circle(buf, 50, 50, 10, color_green);
		printf("Drawing done.\nApplying buffer to the screen.\n");
		rect screenRect={0, 0, surface_screen->w, surface_screen->h};
		surface_apply_notransparency(surface_screen, buf, screenRect);
		while(getchar()!='\n');
	}
	if(strcmp(argv[1], "blur")==0)
	{
		printf("Selected '%s' test.\n", argv[1]);
		printf("Blurring in progress. System isn't crashed or hanged, Surface processes the changes. Please wait...\n");
		surface_blur(surface_screen, 5);
	}
#ifdef MODULE_IMG
	if(strcmp(argv[1], "mask")==0)
	{
		printf("Selected '%s' test.\n", argv[1]);
		#include <png.h>
		surface *surface_load_image(unsigned char *buf, int length);
		surface *sf=surface_load_image(png, png_size);
		rect r={0,0,sf->w,sf->h};
		surface *mask = surface_new();
		surface_default_init(mask, sf->w, sf->h);
		surface_fill_circle(mask, 425, 550, 180, color_black);
		surface_mask(sf, mask);
		surface_apply(surface_screen, sf, r);
	}
#endif
}
