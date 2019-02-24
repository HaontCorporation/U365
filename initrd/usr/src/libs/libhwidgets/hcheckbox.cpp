#include <hwidgets/hcheckbox.hpp>
#include <memory.h>
#include <stdio.h>

hcheckbox::hcheckbox(hwidget* p) : habstractbutton(p) {}
hcheckbox::~hcheckbox(){}

void hcheckbox::ondraw()
{
	if(!surf)
	{
		printf("[HW]: hcheckbox::ondraw() with no surface\n");
		return;
	}
	
	color_rgba c1 = hover ? cs->button_bgcolor_hover : cs->button_bgcolor_active;
	color_rgba c2 = c1;
        c2.a /= 2;
	
	int w = surf->w, h = surf->h;
	int size = 16;
	int rx = 2;
	int ry = (h - size) / 2;
	// Horizontal lines
	rect r = {rx + 1, ry, size - 2, 1};
	surface_draw_rect(surf, c1, r);
	r.y = ry + size - 1;
	surface_draw_rect(surf, c1, r);
	// Vertical lines
	r.x = rx;
	r.y = ry + 1;
	r.w = 1;
	r.h = size - 2;
	surface_draw_rect(surf, c1, r);
	r.x = rx + size - 1;
	surface_draw_rect(surf, c1, r);
	
	// Filling
	if(state)
	{
		r.x = rx + 3;
		r.y = ry + 3;
		r.w = size - 6;
		r.h = size - 6;
		surface_fill_rect(surf, c1, r);
	}

	// Text now
	if(text)
	{
		surface* ts = 0;
		if(ts)
		{
			int tw = ts->w, th = ts->h;
			// Centered for now, we'll make positioning later
			rect tr = {16, (h - th) / 2, tw, th};
			surface_apply(surf, ts, tr);
		}
		else
		{
			printf("[HW]: hcheckbox::ondraw(): Error while rendering text \"%s\"\n", text);
		}
	}
}

void hcheckbox::checked(){}
