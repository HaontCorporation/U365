#include <graphics/surface.h>

#include <graphics/vesa_fb.h>
#include <stdio.h>
#include <arch/i686/devices.h>
#include <arch/i686/mboot.h>
#include <memory.h>
#include <compare.h>
#include <debug.h>
#include <panic.h>
#include <libc/math.h>
#include <vector.h>

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

surface* surface_screen = 0;

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
	//printf("Surface_new()\n");

	surface* obj = malloc(sizeof(surface));

	//printf("Surface: 0x%08x\n", obj);

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
uint8_t surface_blur(surface *obj, uint32_t radius)
{
	//const uint32_t iters = 2;
	//radius /= 2; //We'll do blurring 2 times to smooth resulting image, so to stop detail loss we will divide our radius by 2.
	uint8_t one_radius_alpha_point = 255 / radius;
	/*for(uint32_t iterations = 0; iterations < iters; iterations++)
	{*/
		for(uint32_t j=0; j < obj->h; j++)
			for(uint32_t i=0; i < obj->w; i++)
			{
				color_rgba color = surface_get_pixel(obj, i, j);
				for(uint32_t k=0; k < radius; k++)
				{
					color.a = one_radius_alpha_point * k;
					surface_apply_pixel(obj, color, i+k, j);
					surface_apply_pixel(obj, color, i-k, j);
				}
			}
		for(uint32_t j=0; j < obj->h; j++)
			for(uint32_t i=0; i < obj->w; i++)
			{
				color_rgba color = surface_get_pixel(obj, i, j);
				for(uint32_t k=0; k < radius; k++)
				{
					color.a = one_radius_alpha_point * k;
					surface_apply_pixel(obj, color, i, j+k);
					surface_apply_pixel(obj, color, i, j-k);
				}
			}
	//}
	return 1;
}
uint8_t surface_mask(surface* obj, const surface* mask)
{
	//It's rather simple to implement :)
	if(obj->w != mask->w || obj->h != mask->h)
		return 0;
	for(unsigned y=0; y < obj->h; y++)
	{
		for(unsigned x=0; x < obj->w; x++)
		{
			color_rgba color = surface_get_pixel(obj, x, y);
			color.a = surface_get_pixel(mask, x, y).a;
			surface_set_pixel(obj, color, x, y);
		}
	}
	return 1;
}
surface* surface_add_shadow(surface* sf, uint32_t radius)
{
	//Shadow is basically a black shape with original image shape, we'll blur it later.
	surface *shadow = surface_new();
	surface_default_init(shadow, sf->w, sf->h);
	rect r={0, 0, shadow->w, shadow->h};
	surface_fill_rect(shadow, color_black, r);
	surface_mask(shadow, sf);
	surface *final = surface_new();
	surface_default_init(final, sf->w+5*2, sf->h+5*2);
	rect shadow_place_final = {5, 5, shadow->w, shadow->h};
	rect image_place_final = {0, 0, sf->w, sf->h};
	surface_apply(final, shadow, shadow_place_final);
	surface_blur(final, radius);
	surface_apply(final, sf, image_place_final);
	return final;
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
	obj->w = w;
	obj->h = h;
	return 1;
}

uint8_t surface_set_pixel(surface* obj, color_rgba color, uint32_t x, uint32_t y)
{
	if(obj==surface_screen && kernel_mboot_info->framebuffer_type!=1)
		return 0;
	if(obj && obj->flags & SURFACE_EDITABLE && x < obj->w && y < obj->h)
	{
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
	if(obj->flags & SURFACE_READABLE && x < obj->w && y < obj->h)
	{
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
	return surface_set_pixel(obj, color_mix(surface_get_pixel(obj, x, y), color), x, y);
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

/**
 * Screen surface
 */
void surface_screen_del(surface* obj)
{
	free(obj->data);
}

// Initialize or reinitialize the screen surface pointer
// Please call when the screen definition is changed
void init_surface_screen()
{
	if(surface_screen)
		surface_delete(surface_screen);

	surface_screen        = surface_new();
	surface_screen->flags = SURFACE_READABLE | SURFACE_EDITABLE;
	surface_screen->del   = &surface_screen_del;

	int i;
	int w = vesa_fb_width;
	int h = vesa_fb_height;
	void* scrbuf = vesa_fb_addr;

	surface_screen->data = malloc(h * sizeof(void*));
	surface_screen->w = w;
	surface_screen->h = h;
	surface_screen->bpp = vesa_fb_bpp / 8;
	surface_screen->rb = 2;
	surface_screen->gb = 1;
	surface_screen->bb = 0;
	if(surface_screen->data)
	for(i = 0; i < h; ++i)
	{
		surface_screen->data[i] = (uint8_t*)(scrbuf) + i * vesa_fb_pitch;
	}
	else
	{
		surface_delete(surface_screen);
		surface_screen=0;
		panic("Failed to initialize screen surface.");
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

/*
void sfr_rec(surface* obj, color_rgba src, color_rgba dst, uint32_t x, uint32_t y, uint8_t* bm)
{
	bm[obj->w * y + x] = 1;
	surface_set_pixel(obj, dst, x, y);
	if(x + 1 < obj->w    && !bm[obj->w * y + x + 1  ] && same_color(surface_get_pixel(obj, x + 1, y), src)) sfr_rec(obj, src, dst, x + 1, y, bm);
	if((int)(x - 1) >= 0 && !bm[obj->w * y + x - 1  ] && same_color(surface_get_pixel(obj, x - 1, y), src)) sfr_rec(obj, src, dst, x - 1, y, bm);
	if(y + 1 < obj->h    && !bm[obj->w * (y + 1) + x] && same_color(surface_get_pixel(obj, x, y + 1), src)) sfr_rec(obj, src, dst, x, y + 1, bm);
	if((int)(y - 1) >= 0 && !bm[obj->w * (y - 1) + x] && same_color(surface_get_pixel(obj, x, y - 1), src)) sfr_rec(obj, src, dst, x, y - 1, bm);
}
*/

typedef struct{uint32_t x, y;} point;
void ins_point(vector* obj, point p)
{
	point* n = malloc(8);
	n->x = p.x;
	n->y = p.y;
	vector_push_back(obj, n);
}

void sfr_job(surface* obj, point p, color_rgba src, uint8_t* bm, vector* v)
{
	if(
			p.x < obj->w &&
			(int)(p.x) >= 0 &&
			!bm[obj->w * p.y + p.x] &&
			same_color(surface_get_pixel(obj, p.x, p.y), src))
	{
		ins_point(v, p);
		bm[obj->w * p.y + p.x] = 1;
	}
}

void surface_scale(const surface* obj, surface* target, uint32_t w, uint32_t h)
{
	surface_resize(target, w, h);
	uint32_t x, y, i, j, sw, sh;
	sw = obj->w;
	sh = obj->h;

	for(y = 0; y < h; ++y)
	{
		float svp, evp;
		// Precalculate the row borders here
		// First, find the edge source pixels that enter the target pixel
		svp = h / sh * y;
		evp = h / sh * (y + 1);
		for(x = 0; x < w; ++x)
		{
			float shp, ehp;
			shp = w / sw * x;
			ehp = w / sw * (x + 1);
			float ncr = 0, ncg = 0, ncb = 0, nca = 0, dq = 0;
			if(floor(svp) != floor(evp))
			{
				// Border pixels
				; // AAAAAAAAAAAA WHAT TO DO HERE
				// Internal pixels
				for(i = (uint32_t)(floor(svp)) + 1; i < (uint32_t)(ceil(evp)) - 1; ++i)
				{
					for(j = (uint32_t)(floor(shp)) + 1; j < (uint32_t)(ceil(ehp)) - 1; ++j)
					{
						color_rgba tcolor = surface_get_pixel(obj, j, i);
						ncr += tcolor.r;
						ncg += tcolor.g;
						ncb += tcolor.b;
						nca += tcolor.a;
						++dq;
					}
				}
			}
			;
			color_rgba color = {floor(ncr / dq + 0.5), floor(ncg / dq + 0.5), floor(ncb / dq + 0.5), floor(nca / dq + 0.5)};
			surface_set_pixel(target, color, x, y);
		}
	}
}
