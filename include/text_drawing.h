#ifndef TEXT_DRAWING_H
#define TEXT_DRAWING_H
#include <graphics/surface.h>
#define VGAFNT_CHARACTER_WIDTH   8
#define VGAFNT_CHARACTER_HEIGHT 16

void draw_vgafnt_character(surface *, char, int, int, color_rgba, color_rgba);
void write_vgafnt_string  (surface *, char*, int, int, color_rgba, color_rgba);
#endif