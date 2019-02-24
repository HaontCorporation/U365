#ifndef SURFACE_H
#define SURFACE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rect       rect;
typedef struct color_rgba color_rgba;
typedef struct surface    surface;

typedef enum
{
	SURFACE_READABLE  = 0x0001,
	SURFACE_RESIZABLE = 0x0002,
	SURFACE_EDITABLE  = 0x0004,
	SURFACE_ALPHA     = 0x0008,
} surface_flags_t;

#define SURFACE_DEFAULT_FLAGS (SURFACE_READABLE | SURFACE_RESIZABLE | SURFACE_EDITABLE | SURFACE_ALPHA)

struct rect
{
	int x;
	int y;
	int w;
	int h;
};

struct color_rgba
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct surface
{
		surface* parent;
	void** data;
	uint32_t w;
	uint32_t h;
		// Parental offset
		uint32_t x;
		uint32_t y;
	surface_flags_t flags;

	uint8_t rb;
	uint8_t gb;
	uint8_t bb;
	uint8_t ab;
	uint8_t bpp;

	void (*del)(surface*);
};

/**
 * Basic colors for fast access
 */
extern const color_rgba color_transparency; // 0x00000000
extern const color_rgba color_black;        // 0x000000FF
extern const color_rgba color_red;          // 0xFF0000FF
extern const color_rgba color_green;        // 0x00FF00FF
extern const color_rgba color_blue;         // 0x0000FFFF
extern const color_rgba color_cyan;         // 0x00FFFFFF
extern const color_rgba color_magenta;      // 0xFF00FFFF
extern const color_rgba color_yellow;       // 0xFFFF00FF
extern const color_rgba color_white;        // 0xFFFFFFFF
extern const color_rgba color_gray;         // 0x7F7F7FFF
extern const color_rgba color_light_gray;   // 0xEDEDEDFF

/**
 * Extra colors
 */

extern const color_rgba color_purple;       // 0x7F007FFF
extern const color_rgba color_orange;       // 0xFF7F00FF
extern const color_rgba color_peach;        // 0xFFEEAAFF
extern const color_rgba color_chucknorris;  // 0xCC0000FF
extern const color_rgba color_grass;        // 0x00CC00FF

// Color operations
color_rgba color_mix(color_rgba, color_rgba);
color_rgba color_uint32_to_rgba(uint32_t);
uint32_t   color_rgba_to_uint32(color_rgba);

/**
 * Basic class operations
 */
surface* surface_new();
void     surface_delete     (surface*);
uint8_t  surface_constructor(surface*);
void     surface_destructor (surface*);

/**
 * Methods
 */
surface*   surface_copy       (   const surface* obj);
uint8_t    surface_apply      (         surface* obj, const surface*, rect);
uint8_t    surface_apply_notransparency(surface* obj, const surface*, rect);
surface*   surface_extract    (   const surface* obj, rect);
surface*   surface_subsurface (         surface* obj, rect);
uint8_t    surface_fill_rect  (         surface* obj, color_rgba c, rect r);
uint8_t    surface_draw_rect  (         surface* obj, color_rgba c, rect r);
void       surface_draw_circle(         surface* obj, const uint32_t, const uint32_t,
									  uint16_t, color_rgba);
void       surface_fill_circle(         surface* obj, const uint32_t, const uint32_t,
									  uint16_t, color_rgba);
uint8_t    surface_resize     (         surface* obj, uint32_t w, uint32_t h);
uint8_t    surface_set_pixel  (         surface* obj, color_rgba c, uint32_t x, uint32_t y);
color_rgba surface_get_pixel  (   const surface* obj, uint32_t x, uint32_t y);
uint8_t    surface_apply_pixel(         surface* obj, color_rgba c, uint32_t x, uint32_t y);
void       surface_put_line   (         surface* obj, rect r, color_rgba color);
void       surface_fill_region(         surface* obj, color_rgba c, uint32_t x, uint32_t y);

// Default destructor to be set which will free all data allocated for image holding
void surface_default_destructor(surface* obj);
// Initialize all data fields for ordinary RGBA image holding
void surface_default_init(surface* obj, uint32_t h, uint32_t w);
// Apply a surface to the screen
uint8_t surface_screen_apply(const surface* obj, rect);

#ifdef __cplusplus
}
#endif

#endif // SURFACE_H
