#include "graphics/turtle.h"

#ifdef MODULE_TURTLE
#include <math.h>

int32_t turtle_x = 0;
int32_t turtle_y = 0;
int32_t turtle_angle = 0;
uint8_t turtle_pen = 1;
color_rgba turtle_pen_color = {0, 0, 0, 0};
surface* turtle_target = 0;

void turtle_set_position(int32_t nx, int32_t ny)
{
	turtle_x = nx;
	turtle_y = ny;
}

void turtle_turn_left(int32_t angle)
{
	turtle_angle += angle;
	angle %= 360;
	angle += 360;
	angle %= 360;
}

void turtle_turn_right(int32_t angle)
{
	turtle_turn_left(-angle);
}

void turtle_move_forward(int32_t len)
{
	int32_t tx = len * cos((double)(turtle_angle) * M_PI / 180);
	int32_t ty = len * sin((double)(turtle_angle) * M_PI / 180);

	if(turtle_target && turtle_pen)
	{
		putLine(turtle_x, turtle_y, turtle_x + tx, turtle_y + ty, turtle_pen_color);
	}

	turtle_x += tx;
	turtle_y += ty;
}

void turtle_pen_down()
{
	turtle_pen = 1;
}

void turtle_pen_up()
{
	turtle_pen = 0;
}

void turtle_move_backward(int32_t len)
{
	turtle_move_forward(-len);
}

void turtle_set_target_surface(surface *ts)
{
	turtle_target = ts;
}

void turtle_set_pen_color(color_rgba nc)
{
	turtle_pen_color = nc;
}

void turtle_set_pen_color_raw(uint8_t r, uint8_t g, uint8_t b)
{
	turtle_pen_color.r = r;
	turtle_pen_color.g = g;
	turtle_pen_color.b = b;
}
#endif
