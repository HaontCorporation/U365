#include <tty_old.h>
void tui_draw_box(uint8_t color, int x, int y, int w, int h)
{
	for(int yy=0; yy<h; yy++)
	{
		for(int xx=0; xx<w; xx++)
		{
			tty_putentryat(' ', x+xx, y+yy, color);
		}
	}
}
void tui_bordered_box(uint8_t color, int x, int y, int w, int h/*uint8_t thin*/)
{
    tui_draw_box(color, x, y, w, h);
    for(int xx=0; xx<w; xx++)
    {
        tty_putentryat(0xCD, x+xx, y, color);
        tty_putentryat(0xCD, x+xx, y+h-1, color);
    }
    for(int yy=0; yy<h; yy++)
    {
        tty_putentryat(0xBA, x, y+yy, color);
        tty_putentryat(0xBA, x+w-1, y+yy, color);
    }
    tty_putentryat(0xC9, x, y, color);
    tty_putentryat(0xBB, x+w-1, y, color);
    tty_putentryat(0xC8, x, y+h-1, color);
    tty_putentryat(0xBC, x+w-1, y+h-1, color);
}

void tui_draw_text(uint8_t color, int x, int y, const char* text)
{
	int old_x = x;
	for(int i=0; text[i]; i++)
	{
		switch(text[i])
		{
			case '\n': y++; x = old_x; break;
			case '\r': x = old_x; break;
			default: x++; tty_putentryat(text[i], x, y, color); break;
		}
	}
}
void tui_draw_lim_text(uint8_t color, int x, int y, int w, int h, const char* text, unsigned length)
{
    int old_x = x, old_y = y;
    for(unsigned i=0; i<length; i++)
    {
        switch(text[i])
        {
            case '\n': y++; x = old_x; break;
            case '\r': x = old_x; break;
            default: x++; tty_putentryat(text[i], x, y, color); break;
        }
        if(x >= old_x+w)
        {
            x = old_x;
            if(old_y+y >= h) return; else y++;
        }
    }
}

void tui_fill_area(uint8_t c, int x, int y, int w, int h, uint8_t color)
{
    for(int yy=0; yy<h; yy++)
    {
        for(int xx=0; xx<w; xx++)
        {
            tty_putentryat(c, x+xx, y+yy, color);
        }
    }
}
