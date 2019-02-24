#include <hwidgets/hwidget.hpp>
#include <memory.h>
#include <stdio.h>

colorscheme colorscheme_default;

void colorscheme_setdefault()
{
	colorscheme_default.window_bgcolor        = color_peach;
	colorscheme_default.button_bgcolor        = color_orange;
	colorscheme_default.button_bgcolor_hover  = color_white;
	colorscheme_default.button_bgcolor_active = {127, 0, 0, 255};
	colorscheme_default.border                = color_gray;
	colorscheme_default.border_hover          = color_gray;
	colorscheme_default.border_active         = color_gray;
	colorscheme_default.button_fgcolor        = color_black;
	colorscheme_default.fontdata              = 0;
}

hwidget::hwidget(hwidget* p)
{
	colorscheme_setdefault();

	setParent(p);
	if(p)
		cs = p->cs;
	else
		cs = &colorscheme_default;
        minimumWidth  = 0;
        minimumHeight = 0;
        maximumWidth  = 0x7FFFFFFF;
        maximumHeight = 0x7FFFFFFF;
}

hwidget::~hwidget()
{
	for(size_t i = 0; i < children.size(); ++i)
		delete children[i];
}

void hwidget::setParent(hwidget* p)
{
	if(p != parent)
	{
                rect r = {0, 0, (int)(minimumWidth), (int)(minimumHeight)};
                // Clean up old parent's mess
                if(surf)
                {
                    r.w = surf->w;
                    r.h = surf->h;
                }
		if(parent)
		{
			//vector* ch = children;
			//vector_erase_by_value(ch, obj);
		}
		if(surf)
                {
                        //surface_delete(surf);
                        free(surf);
                }
                // Now, create the fresh mess for the new parent
		parent = p;
		if(parent)
		{
                        parent->children.push_back(this);
			if(surf)
			{
				//surface_delete(surf);
				free(surf);
			}
			surf = surface_subsurface(surf, r);
		}
		else
                {
                    surf = surface_new();
                    surface_default_init(surf, r.w, r.h);
                }
	}
}

void hwidget::draw()
{
	ondraw();
	for(size_t i = 0; i < children.size(); ++i)
		children[i]->draw();
}

void hwidget::resize(uint32_t w, uint32_t h)
{
	if(w > maximumWidth)
		w = maximumWidth;
	if(w < minimumWidth)
		w = minimumWidth;
	if(h > maximumHeight)
		h = maximumHeight;
	if(h < minimumHeight)
		h = minimumHeight;

	if(surf)
	{
		surface_resize(surf, w, h);
	}
	else
	{
		if(parent)
		{
			rect r = {0, 0, (int)(w), (int)(h)};
			surf = surface_subsurface(parent->surf, r);
		}
		else
		{
			surf = surface_new();
			surface_default_init(surf, w, h);
		}
	}
	onresize(w, h);
}

void hwidget::setPosition(uint32_t x, uint32_t y)
{
	if(parent)
	{
		surf->x = x;
		surf->y = y;
	}
}

const surface* hwidget::getSurface()
{
	return surf;
}

// Handlers
void hwidget::ondraw()
{
	rect r = {0, 0, (int)(surf->w), (int)(surf->h)};
	surface_draw_rect(surf, cs->border, r);
	r.x += 1;
	r.y += 1;
	r.h -= 2;
	r.w -= 2;
	surface_fill_rect(surf, cs->window_bgcolor, r);
}

bool hwidget::onclick    (){return 0;}
bool hwidget::onmouseover(){return 0;}
bool hwidget::onmouseout (){return 0;}
bool hwidget::onmousedown(){return 0;}
bool hwidget::onmouseup  (){return 0;}
bool hwidget::onfocus    (){return 0;}
void hwidget::onunfocus  (){}
void hwidget::onresize(uint32_t, uint32_t){}

