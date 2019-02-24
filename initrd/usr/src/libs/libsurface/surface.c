#include <surface.h>

#include <stdio.h>
#include <memory.h>
#include <syscalls.h>
#include <compare.h>
#include <math.h>

const color_rgba color_transparency = {0x00, 0x00, 0x00, 0x00};
const color_rgba color_black        = {0x00, 0x00, 0x00, 0xFF};
const color_rgba color_red          = {0xFF, 0x00, 0x00, 0xFF};
const color_rgba color_green        = {0x00, 0xFF, 0x00, 0xFF};
const color_rgba color_blue         = {0x00, 0x00, 0xFF, 0xFF};
const color_rgba color_cyan         = {0x00, 0xFF, 0xFF, 0xFF};
const color_rgba color_magenta      = {0xFF, 0x00, 0xFF, 0xFF};
const color_rgba color_yellow       = {0xFF, 0xFF, 0x00, 0xFF};
const color_rgba color_gray         = {0x77, 0x77, 0x77, 0xFF};
const color_rgba color_light_gray   = {0xED, 0xED, 0xED, 0xFF};
const color_rgba color_white        = {0xFF, 0xFF, 0xFF, 0xFF};

const color_rgba color_purple       = {0x7F, 0x00, 0x7F, 0xFF};
const color_rgba color_orange       = {0xFF, 0x7F, 0x00, 0xFF};
const color_rgba color_peach        = {0xFF, 0xEE, 0xAA, 0xFF};
const color_rgba color_chucknorris  = {0xCC, 0x00, 0x00, 0xFF};
const color_rgba color_grass        = {0x00, 0xCC, 0x00, 0xFF};

uint8_t channel_mix(uint8_t ca, uint8_t cb, uint8_t aa, uint8_t ab)
{
	double faa = (double)(aa) / 255.0;
	double fab = (double)(ab) / 255.0;
	return (double)(ca) * (faa - fab) + (double)(cb) * fab;
}

uint8_t alpha_mix(uint8_t aa, uint8_t ab)
{
	double faa = (double)(aa) / 255;
	double fab = (double)(ab) / 255;
	double far = 1 - (1 - faa) * (1 - fab);
	return far * 255;
}

color_rgba color_mix(color_rgba a, color_rgba b)
{
	color_rgba result;
	result.r = channel_mix(a.r, b.r, a.a, b.a);
	result.g = channel_mix(a.g, b.g, a.a, b.a);
	result.b = channel_mix(a.b, b.b, a.a, b.a);
	result.a = alpha_mix(a.a, b.a);
	return result;
}

color_rgba color_uint32_to_rgba(uint32_t d)
{
	color_rgba result;
	result.r = d >> 6;
	result.g = (d >> 4) & 0xFF;
	result.b = (d >> 2) & 0xFF;
	result.a = d & 0xFF;
	return result;
}

uint32_t color_rgba_to_uint32(color_rgba c)
{
	return ((c.r << 6) | (c.g << 4) | (c.b << 2) | (c.a));
}

/**
 * Basic class operations
 */
surface* surface_new()
{
	surface* obj = malloc(sizeof(surface));
	if(obj && surface_constructor(obj))
		return obj;
	return 0;
}

void surface_delete(surface* obj)
{
	if(obj)
	{
		if(obj->del)
			obj->del(obj);
		free(obj);
	}
}

uint8_t surface_constructor(surface* obj)
{
	obj->data  = 0;
	obj->w     = 0;
	obj->h     = 0;
	obj->flags = 0;
	obj->rb    = 0;
	obj->gb    = 0;
	obj->bb    = 0;
	obj->ab    = 0;
	return 1;
}

/**
 * Methods
 */
surface* surface_copy(const surface* obj)
{
	rect r = {0, 0, obj->w, obj->h};
	return surface_extract(obj, r);
}

uint8_t surface_apply(surface* obj, const surface* src, rect r)
{
	if(!obj || !src || !(obj->flags & SURFACE_EDITABLE) || r.w <= 0 || r.h <= 0)
		return 0;
	uint32_t i, j;

	for(i = 0; i < (uint32_t)(r.h) && i + (uint32_t)(r.y) < obj->h; ++i)
	{
		for(j = 0; j < (uint32_t)(r.w) && j < (uint32_t)(r.x) + obj->w; ++j)
		{
			color_rgba tmp = color_mix(surface_get_pixel(obj, r.x + j, r.y + i), surface_get_pixel(src, j, i));
			surface_apply_pixel(obj, tmp, r.x + j, r.y + i);
		}
	}
	return 1;
}
//Use when you don't want any alpha channel. It works faster.
uint8_t surface_apply_notransparency(surface* obj, const surface* src, rect r)
{
	if(!obj || !src || !(obj->flags & SURFACE_EDITABLE) || r.w <= 0 || r.h <= 0)
		return 0;
	uint32_t i, j;

	for(i = 0; i < (uint32_t)(r.h) && i + (uint32_t)(r.y) < obj->h; ++i)
	{
		for(j = 0; j < (uint32_t)(r.w) && j < (uint32_t)(r.x) + obj->w; ++j)
		{
			surface_set_pixel(obj, surface_get_pixel(src, j, i), r.x + j, r.y + i);
		}
	}
	return 1;
}

surface* surface_extract(const surface* obj, rect r)
{
	if(!obj || r.w <= 0 || r.h <= 0)
		return 0;
	surface* result = surface_new();
	surface_default_init(result, r.w, r.h);
	uint32_t i, j;
	for(i = 0; i < (uint32_t)(r.h) && i < (uint32_t)(r.y) + obj->h; ++i)
	{
		for(j = 0; j < (uint32_t)(r.w) && j < (uint32_t)(r.x) + obj->w; ++j)
		{
			surface_set_pixel(result, surface_get_pixel(obj, r.x + j, r.y + i), j, i);
		}
	}
	return result;
}

surface* surface_subsurface(surface* src, rect r)
{
    if(!src || r.x < 0 || r.y < 0 || (uint32_t)(r.x + r.w) > src->w || (uint32_t)(r.y + r.h) > src->h)
        return 0;
    surface* result = surface_new();
    result->parent = src;
    result->x = r.x;
    result->y = r.y;
    result->w = r.w;
    result->h = r.h;
    result->flags = src->flags;
    return result;
}

uint8_t surface_fill_rect(surface* obj, color_rgba color, rect r)
{
	if(!obj || !(obj->flags & SURFACE_EDITABLE))
		return 0;

	for(uint32_t i = 0; i < (uint32_t)(r.h); i++)
	{
		for(uint32_t j = 0; j < (uint32_t)(r.w); j++)
		{
			if(color.a == 255)
				surface_set_pixel  (obj, color, r.x + j, r.y + i);
			else
				surface_apply_pixel(obj, color, r.x + j, r.y + i);
		}
	}
	return 1;
}

uint8_t surface_draw_rect(surface* obj, color_rgba color, rect r)
{
	if(!obj || !(obj->flags & SURFACE_EDITABLE))
		return 0;

	for(uint32_t i = 0; i < (uint32_t)(r.h) && r.y + i < obj->h; i++)
	{
		if(color.a == 255)
                {
			surface_set_pixel  (obj, color, r.x,           r.y + i);
                        surface_set_pixel  (obj, color, r.x + r.w - 1, r.y + i);
                }
		else
                {
			surface_apply_pixel(obj, color, r.x,           r.y + i);
                        surface_apply_pixel(obj, color, r.x + r.w - 1, r.y + i);
                }
	}
	for(uint32_t i = 0; i < (uint32_t)(r.w) && r.x + i < obj->w; i++)
	{
		if(color.a == 255)
                {
			surface_set_pixel  (obj, color, r.x + i, r.y);
                        surface_set_pixel  (obj, color, r.x + i, r.y + r.h - 1);
                }
		else
                {
			surface_apply_pixel(obj, color, r.x + i, r.y);
                        surface_apply_pixel(obj, color, r.x + i, r.y + r.h - 1);
                }
	}
	return 1;
}

void surface_fill_circle(surface* srf, const uint32_t x, const uint32_t y,
						 uint16_t radius, color_rgba color)
{
	while(radius > 0)
	{
		unsigned xm=0;

		int delta=1-2*radius, error=0, ym=radius;

		while(1)
		{
			surface_set_pixel(srf, color, x+xm, y+ym);
			surface_set_pixel(srf, color, x-xm, y+ym);
			surface_set_pixel(srf, color, x+xm, y-ym);
			surface_set_pixel(srf, color, x-xm, y-ym);
			surface_set_pixel(srf, color, x+xm-1, y+ym);
			surface_set_pixel(srf, color, x-xm+1, y+ym);
			surface_set_pixel(srf, color, x+xm-1, y-ym);
			surface_set_pixel(srf, color, x-xm+1, y-ym);

			error = 2 * (delta + ym) - 1;

			if ((delta < 0) && (error <= 0) && ym != 0)
			{
				delta += 2 * ++xm + 1;
				continue;
			}

			error = 2 * (delta - xm) - 1;

			if ((delta > 0) && (error > 0) && ym != 0)
			{
				delta += 1 - 2 * --ym;
				continue;
			}

			xm++;

			delta += 2 * (xm - ym);

			if(ym == 0) break;
			ym--;
		}
		radius--;
	}
}

void surface_draw_circle(surface* srf, const uint32_t x, const uint32_t y,
						 uint16_t radius, color_rgba color)
{
	unsigned xm=0;

	int delta=1-2*radius, error=0, ym=radius;

	while(1)
	{
		surface_set_pixel(srf, color, x+xm, y+ym);
		surface_set_pixel(srf, color, x-xm, y+ym);
		surface_set_pixel(srf, color, x+xm, y-ym);
		surface_set_pixel(srf, color, x-xm, y-ym);

		error = 2 * (delta + ym) - 1;

		if ((delta < 0) && (error <= 0) && ym !=0)
		{
			delta += 2 * ++xm + 1;
			continue;
		}

		error = 2 * (delta - xm) - 1;

		if ((delta > 0) && (error > 0) && ym != 0)
		{
			delta += 1 - 2 * --ym;
			continue;
		}

		xm++;

		delta += 2 * (xm - ym);

		if(ym == 0) return;

		ym--;
	}
}

uint8_t surface_resize(surface* obj, uint32_t w, uint32_t h)
{
	if(!obj || !(obj->flags & SURFACE_RESIZABLE))
		return 0;
        if(obj->parent && (obj->x + w > obj->parent->w || obj->y + h > obj->parent->h))
            return 0;
        if(!obj->parent)
        {
            uint32_t i;
            obj->data = realloc(obj->data, obj->bpp * h);
            if(!obj->data)
                    return 0;

            for(i = obj->h; i < h; ++i)
                    obj->data[i] = 0;
            for(i = 0; i < h; ++i)
            {
                    obj->data[i] = realloc(obj->data[i], w * obj->bpp);
                    if(!obj->data[i])
                            return 0;
            }
        }
	obj->w = w;
	obj->h = h;
	return 1;
}
uint8_t surface_set_pixel(surface* obj, color_rgba color, uint32_t x, uint32_t y)
{
	if(obj && obj->flags & SURFACE_EDITABLE && x < obj->w && y < obj->h)
	{
                if(obj->parent)
                    return surface_set_pixel(obj->parent, color, x + obj->x, y + obj->y);
		uint32_t off=x * obj->bpp;
		if(obj->rb < obj->bpp) ((uint8_t**)(obj->data))[y][off + obj->rb] = color.r;
		if(obj->gb < obj->bpp) ((uint8_t**)(obj->data))[y][off + obj->gb] = color.g;
		if(obj->bb < obj->bpp) ((uint8_t**)(obj->data))[y][off + obj->bb] = color.b;
		if(obj->flags & SURFACE_ALPHA && obj->ab < obj->bpp)
			((uint8_t**)(obj->data))[y][off + obj->ab] = color.a;
		return 1;
	}
	return 0;
}

color_rgba surface_get_pixel(const surface* obj, uint32_t x, uint32_t y)
{
	color_rgba result = {0, 0, 0, 0};
	if(obj && obj->flags & SURFACE_READABLE && x < obj->w && y < obj->h)
	{
                if(obj->parent)
                    return surface_get_pixel(obj->parent, x + obj->x, y + obj->y);
		uint32_t off=x * obj->bpp;
		if(obj->rb < obj->bpp) result.r = ((uint8_t**)(obj->data))[y][off + obj->rb];
		if(obj->gb < obj->bpp) result.g = ((uint8_t**)(obj->data))[y][off + obj->gb];
		if(obj->bb < obj->bpp) result.b = ((uint8_t**)(obj->data))[y][off + obj->bb];
		if(obj->flags & SURFACE_ALPHA && obj->ab < obj->bpp)
			result.a = ((uint8_t**)(obj->data))[y][off + obj->ab];
		else
			result.a = 255;
	}
	return result;
}

uint8_t surface_apply_pixel(surface* obj, color_rgba color, uint32_t x, uint32_t y)
{
	if(!obj || !((obj->flags & SURFACE_EDITABLE) && (obj->flags & SURFACE_READABLE)))
		return 0;
	if(color.a)
		return surface_set_pixel(obj, color_mix(surface_get_pixel(obj, x, y), color), x, y);
	return 1;
}

/**
 * Stuff
 */
// Default destructor to be set which will free all data allocated for image holding
void surface_default_destructor(surface* obj)
{
	uint32_t i;
	for(i = 0; i < obj->h; ++i)
	{
		free(obj->data[i]);
	}
	free(obj->data);
}
// Initialize all data fields for ordinary RGBA image holding
void surface_default_init(surface* obj, uint32_t w, uint32_t h)
{
	obj->data  = calloc(sizeof(void*) * h, 1); //will be nulled :)
	obj->w     = w;
	obj->h     = h;
	obj->flags = SURFACE_DEFAULT_FLAGS;
	obj->rb    = 3;
	obj->gb    = 2;
	obj->bb    = 1;
	obj->ab    = 0;
	obj->bpp   = 4;
	obj->del   = &surface_default_destructor;
	uint32_t i;
	for(i = 0; i < h; ++i)
	{
		obj->data[i] = calloc(w * 4, 1);
	}
}


void surface_put_line (surface* obj, rect r, color_rgba color)
{
	if(!obj || (!obj->w && !obj->h))
		return;
	if(abs(r.h) > abs(r.w))
	{
		double kx = (double)(r.w) / r.h;
		for(int y = 0; y != r.h; y += sign(r.h))
		{
			double tx = y * kx;
			int x = tx + 0.5;
			surface_set_pixel(obj, color, r.x + x, r.y + y);
		}
	}
	else
	{
		double ky = (double)(r.h) / r.w;
		for(int x = 0; x != r.w; x += sign(r.w))
		{
			double ty = x * ky;
			int y = ty + 0.5;
			surface_set_pixel(obj, color, r.x + x, r.y + y);
		}
	}
}

int8_t same_color(color_rgba a, color_rgba b)
{
	return (a.r == b.r && a.b == b.b && a.g == b.g && a.a == b.a);
}

int color_delta(color_rgba a, color_rgba b)
{
	return ((int)(0) + abs(a.r - b.r) + abs(a.g - b.g) + abs(a.b - b.b)/* + abs(a.a - b.a)*/);
}


// LIBRARY-SPECIFIC

uint8_t surface_screen_apply(const surface* obj, rect r)
{
    return syscall(SYS_T_screen_apply, obj, r);
}
