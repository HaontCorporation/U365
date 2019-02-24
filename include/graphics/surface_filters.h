#include <stdint.h>
#include <graphics/surface.h>
typedef struct surface_filter {
	const char *name; //Filters are added by add_filter. They are only one for one Surface copy. Filters are accessed by names.
	void (*process_pixel)(uint32_t x, uint32_t y, color_rgba old_color, ...);
} surface_filter;