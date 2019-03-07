
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <tty.h>
#include <vbe.h>

#define TERM_DEFAULT_COLOR 0x07 //Default terminal color.

int tty_w; //Variables.
int tty_h;
int tty_x, tty_y;
uint8_t tty_color;
uint8_t tty_drawCharBg = 0;
uint32_t glyphs_cnt, chr_w, chr_h;
volatile uint8_t tty_feedback = 1;

uint8_t *tty_buf; //Terminal character buffer.
void vesa_putc(uint8_t c, int x, int y, uint32_t fg, uint32_t bg)
{
	int cx,cy;
	int mask[8]={128,64,32,16,8,4,2,1};
	unsigned char *glyph=(uint8_t*)vgafnt+(int)c*16;
	for(cy=0;cy<chr_h;cy++){
		for(cx=0;cx<chr_w;cx++){
			if(glyph[cy]&mask[cx]) putPix(x+cx,y+cy,fg);
			else if(tty_drawCharBg) putPix(x+cx,y+cy,bg);
		}
	}

}

//Here starts the code.
void initTTY() //Initializing the terminal device.
{
	tty_x = tty_y = 0;
	chr_w = 8;
	chr_h = 16;
    tty_w = 1024 / chr_w;
    tty_h = 768 / chr_h;
	tty_color=TERM_DEFAULT_COLOR;
	//tty_buf = 0xB8000;
	//memset(tty_buf, 0, tty_h*tty_w*2*2);
	//for(int y=0; y<tty_h; y++)
	//	for(int x=0; x<tty_w; x++)
	//		tty_putentryat(' ', x, y, 0x00);
}
//copied from osdev.org.
//Needed for cursor updating in text mode.
 /* void update_cursor(int col, int row)
  * by Dark Fiber
  */
void update_cursor(int col, int row)
{
	unsigned short position=(row*80) + col;

	// cursor LOW port to vga INDEX register
	outb(0x3D4, 0x0F);
	outb(0x3D5, (unsigned char)(position&0xFF));
	// cursor HIGH port to vga INDEX register
	outb(0x3D4, 0x0E);
	outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}

//This function does actual drawing.
void tty_putentryat(uint8_t c, int x, int y, uint8_t color)
{
	//We need to put our character to the buffer, or it will erase when screen will scroll.
	//tty_buf[(y*tty_w+x)*2] = c;
	//tty_buf[(y*tty_w+x)*2+1] = color;
	//if(kernel_mboot_info->framebuffer_type!=1) //we're in text mode, so we need to write text to it
	//{
	//	//We don't need any color conversion in text mode, so we'll compute text mode framebuffer offset and write 16-bit value to it. High 8 bits are color, low 8 bits are character ASCII code.
	//	unsigned int where = y*tty_w+x;
	//	uint16_t c16 = c;
	//	uint16_t color16 = color;
	//	uint16_t val = c16 | color16 << 8;
	//	uint16_t *buf=(uint16_t*)0xB8000;
	//	buf[where]  =val;
	//	return;
	//}
	////We have 16-color terminal. Our colors are encoded in one 8-bit value.
	////Low part is foreground, high part is background.
	static uint32_t c16pal[16]={BLACK,BLUE,GREEN,CYAN,RED,MAGENTA,BROWN,LIGHT_GREY,DARK_GREY,LIGHT_BLUE,LIGHT_GREEN,LIGHT_CYAN,LIGHT_RED,LIGHT_MAGENTA,LIGHT_BROWN,WHITE};
	vesa_putc(c,x*chr_w,y*chr_h,c16pal[color<<4>>4],c16pal[color>>4]); //Calling vesa_putc to actually draw character on the screen.
}

void terminal_scroll()
{
	//This function scrolls the screen one character up.
	tty_y=tty_h;
	//Move every line 1 line up.
	for(int y = 0; y <= tty_h; y++)
	{
		for(int x = 0; x < tty_w; x++)
		{
			tty_putentryat(tty_buf[(y * tty_w + x) * 2], x, y - 1, tty_buf[(y * tty_w + x) * 2 + 1]);
		}
	}
	//if(kernel_mboot_info->framebuffer_type!=1) //we're in text mode, so we need to write text to it
	//{
	//	memmove((void*)0xB8000-80*2, (void*)0xB8000, 80 * 25 * 2);
	//	for(int i=0; i<tty_w; i++)
	//	tty_putentryat(' ',i,tty_h,0x00);
	//	update_cursor(tty_x, tty_y);
	//	return;
	//}
	//memmove(vesa_fb_addr-vesa_fb_width*16*4, vesa_fb_addr, vesa_fb_width * vesa_fb_height * 4);/*
	for(int y = 0; y <= tty_h; y++)
	{
		for(int x = 0; x < tty_w; x++)
		{
			tty_putentryat(tty_buf[(y * tty_w + x) * 2], x, y, tty_buf[(y * tty_w + x) * 2+3]);
		}
	}
	for(int i = 0; i < tty_h; i++){
		for (int m = 0; m < tty_w; m++){
			tty_buf[i * tty_w + m] = tty_buf[(i + 1) * tty_w + m];
		}
	}
	for(int y = 0; y <= tty_h; y++)
	{
		for(int x = 0; x < tty_w; x++)
		{
			tty_putentryat(tty_buf[(y * tty_w + x) * 2], x, y, tty_buf[(y * tty_w + x) * 2+3]);
		}
	}
	//And then fill the last line on the screen with nothing.
	for(int i=0; i<tty_w; i++)
		tty_putentryat(' ',i,tty_h,0x00);
}
//This function writes 1 character to the screen. It starts new line on \n, returns to start of the line on \r and wipes previous character on \b.
void tty_putchar(char c)
{
	#ifdef CRASH_DEBUGGING
	//If it's defined, current OS version doesn't work. We use ANSI escape codes to make system output differ from emulator output.
	outb(0xe9, '\e'); //Make our text bold.
	outb(0xe9, '[');
	outb(0xe9, '1');
	outb(0xe9, 'm');

	outb(0xe9, c); //Write the character.
	outb(0xe9, '\e'); //Reset all attributes.
	outb(0xe9, '[');
	outb(0xe9, '0');
	outb(0xe9, 'm');

	return;
	#endif
	if(tty_x + 1 >= tty_w)
	{
		tty_x = 0;
		if(tty_y < tty_h)
			tty_y++;
		else
			terminal_scroll();
	}
	switch(c)
	{
		case '\n': if(tty_y++ >= tty_h){ terminal_scroll(); } /* fall through */
		case '\r': tty_x = 0; break;
		case '\b': tty_putentryat(' ',tty_x,tty_y,tty_color); if(tty_x == 0 && tty_y > 0) { tty_y--; break; } tty_x--; break;
		case '\t': do tty_putentryat(' ', tty_x++, tty_y, tty_color); while(tty_x % 4); break;
		default: tty_putentryat(c,tty_x++,tty_y,tty_color);
	}
	update_cursor(tty_x,tty_y);
}
//These functions are self-documenting.
void tty_setcolor(uint8_t color)
{
	tty_color=color;
}

uint8_t tty_mkcolor(uint8_t fg, uint8_t bg)
{
	return fg | bg << 4;
}

//Very big and complicated function. Writes an entire string with ANSI escape sequences to the screen.
void tty_wrstr(const char* str)
{
	while(*str)
	{
		if(*str!='\e') //If it's not start of ANSI escape sequence, just print this character on the screen.
			tty_putchar(*str);
		else
		{
			//We'll going to parse ANSI escape sequence.
			if(*++str=='[') //[ is 2nd escape sequence start characters. It ends on 'm'.
			{
				switch(atoi(str+1)) //We're using a big switch-case construction there.
				{
					case 0:  tty_setcolor(0x07); break;
					case 1:  tty_setcolor((tty_color<8) ? tty_color+0x08 : tty_color); break;
					case 2:  tty_setcolor((tty_color>8) ? tty_color-0x08 : tty_color); break;
					case 7:  tty_setcolor(((tty_color << 4) | (tty_color >> 4))); break;
					case 8:  tty_setcolor(tty_mkcolor(tty_color>>4, tty_color>>4)); break;
					case 21: tty_setcolor((tty_color>8) ? tty_color-0x08 : tty_color); break;
					case 22: tty_setcolor((tty_color<8) ? tty_color+0x08 : tty_color); break;
					case 27: tty_color = tty_mkcolor(tty_color<<4, tty_color>>4); break;
					case 28: tty_setcolor(TERM_DEFAULT_COLOR); break;
					//Color setting, yes, a lot of code.
					//foreground
					case 39: tty_setcolor(tty_mkcolor(7, tty_color>>4)); break;
					case 30: tty_setcolor(tty_mkcolor(0, tty_color>>4)); break;
					case 31: tty_setcolor(tty_mkcolor(4, tty_color>>4)); break;
					case 32: tty_setcolor(tty_mkcolor(2, tty_color>>4)); break;
					case 33: tty_setcolor(tty_mkcolor(6, tty_color>>4)); break;
					case 34: tty_setcolor(tty_mkcolor(1, tty_color>>4)); break;
					case 35: tty_setcolor(tty_mkcolor(5, tty_color>>4)); break;
					case 36: tty_setcolor(tty_mkcolor(3, tty_color>>4)); break;
					case 37: tty_setcolor(tty_mkcolor(7, tty_color>>4)); break;
					case 90: tty_setcolor(tty_mkcolor(8, tty_color>>4)); break;
					case 91: tty_setcolor(tty_mkcolor(12, tty_color>>4)); break;
					case 92: tty_setcolor(tty_mkcolor(10, tty_color>>4)); break;
					case 93: tty_setcolor(tty_mkcolor(14, tty_color>>4)); break;
					case 94: tty_setcolor(tty_mkcolor(9, tty_color>>4)); break;
					case 95: tty_setcolor(tty_mkcolor(13, tty_color>>4)); break;
					case 96: tty_setcolor(tty_mkcolor(11, tty_color>>4)); break;
					case 97: tty_setcolor(tty_mkcolor(15, tty_color>>4)); break;
					//background
					case 49: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 0)); break;
					case 40: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 0)); break;
					case 41: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 4)); break;
					case 42: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 2)); break;
					case 43: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 14)); break;
					case 44: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 1)); break;
					case 45: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 5)); break;
					case 46: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 3)); break;
					case 47: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 7)); break;
					case 100: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 8)); break;
					case 101: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 12)); break;
					case 102: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 10)); break;
					case 103: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 14)); break;
					case 104: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 9)); break;
					case 105: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 13)); break;
					case 106: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 11)); break;
					case 107: tty_setcolor(tty_mkcolor(tty_color<<4>>4, 15)); break;
				}
				for(; *str!='m'; str++) //Skip our number part and return to normal text.
					;
			}
		}
        str++;
	}
}

void tty_goToPos(int x, int y) //Self-documenting.
{
	tty_x=x;
	tty_y=y;
}