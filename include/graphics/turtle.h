#ifndef TURTLE_H
#define TURTLE_H

#include <math.h>
#include <stdint.h>
#include <graphics/surface.h>

#define M_PI 3.1415962

extern int32_t turtle_x;
extern int32_t turtle_y;
extern int32_t turtle_angle;
extern surface* turtle_target;

void turtle_set_angle    (int32_t na);
void turtle_set_position (int32_t nx, int32_t ny);
void turtle_turn_right   (int32_t a);
void turtle_turn_left    (int32_t a);
void turtle_move_forward (int32_t l);
void turtle_move_backward(int32_t l);
void turtle_pen_down();
void turtle_pen_up();
void turtle_set_pen_color(color_rgba nc);
void turtle_set_pen_color_raw(uint8_t r, uint8_t g, uint8_t b);
void turtle_set_target_surface(surface* ts);

#endif // TURTLE_H
