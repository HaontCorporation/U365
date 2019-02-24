#ifndef TRUETYPE_H
#define TRUETYPE_H
#include <graphics/surface.h>
enum ttf_text_direction {
	DIRECTION_LEFT,
	DIRECTION_CENTER,
	DIRECTION_RIGHT,
};
surface *draw_ttf_char(unsigned char *, int, int, color_rgba);
surface *draw_ttf_string(unsigned char *, const char *, int, color_rgba);
surface *ttf_write_text(unsigned char *, const char*, int, color_rgba, enum ttf_text_direction);
#endif //TRUETYPE_H