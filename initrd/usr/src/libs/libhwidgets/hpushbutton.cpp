#include <hwidgets/hpushbutton.hpp>
#include <memory.h>
#include <stdio.h>

hpushbutton::hpushbutton(hwidget* p) : habstractbutton(p) {}
hpushbutton::~hpushbutton(){}

void hpushbutton::ondraw()
{
	if(!surf)
	{
		printf("[HW]: hpushbutton::ondraw() with no surface\n");
		return;
	}
	int w = surf->w, h = surf->h;
	color_rgba border     = active ? cs->border_active         : cs->border;
	color_rgba background = active ? cs->button_bgcolor_active : cs->button_bgcolor;
        color_rgba b2 = border;
        b2.a /= 2;

        // Make a fancy rounded rectangle
	rect r = {1, 1, (int)(w - 2), (int)(h - 2)};
	surface_fill_rect(surf, background, r);
	r.x = 0;
	r.w = 1;
	surface_draw_rect(surf, border, r);
	r.x = w - 1;
	surface_draw_rect(surf, border, r);
	r.x = 1;
	r.y = 0;
	r.w = w - 2;
	r.h = 1;
	surface_draw_rect(surf, border, r);
	r.y = h - 1;
	surface_draw_rect(surf, border, r);
	/*
	printf("Fancy corners\n");
        surface_apply_pixel(surf, b2,     1, 1);
        surface_apply_pixel(surf, b2,     1, h - 2);
        surface_apply_pixel(surf, b2, w - 2, 1);
        surface_apply_pixel(surf, b2, w - 2, h - 2);
        */

	// Text now
	if(text)
	{
		surface* ts = 0;
		if(ts)
		{
			int tw = ts->w, th = ts->h;
			// Centered for now, we'll make positioning later
			rect tr = {(w - tw) / 2, (h - th) / 2, tw, th};
			surface_apply(surf, ts, tr);
		}
		else
		{
			printf("[HW]: hpushbutton::ondraw(): Error while rendering text \"%s\"\n", text);
		}
	}
}
