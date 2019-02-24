#ifndef SURFACE_K_H
#define SURFACE2_K_H

#include <stdint.h>
#include <surface.h>

#ifdef __cplusplus
extern "C" {
#endif

extern surface* surface_screen;

uint8_t    surface_resize     (         surface* obj, uint32_t w, uint32_t h);
uint8_t    surface_set_pixel  (         surface* obj, color_rgba c, uint32_t x, uint32_t y);
color_rgba surface_get_pixel  (   const surface* obj, uint32_t x, uint32_t y);
uint8_t    surface_apply_pixel(         surface* obj, color_rgba c, uint32_t x, uint32_t y);
void       surface_scale      (   const surface* obj, surface*, uint32_t, uint32_t);
uint8_t    surface_blur       (         surface* obj, uint32_t s);
uint8_t    surface_mask       (         surface* obj, const surface* mask);
void       surface_put_line   (         surface* obj, rect r, color_rgba color);
void       surface_fill_region(         surface* obj, color_rgba c, uint32_t x, uint32_t y);

// Get the screen surface (Unresizable and should be reinitialized when the screen is resized)
void init_surface_screen();

#ifdef __cplusplus
}
#endif

#endif // SURFACE_K_H
