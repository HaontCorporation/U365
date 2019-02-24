#include <graphics/surface.h>

// Just easy math
color_rgba transcolor(color_rgba c1, color_rgba c2, float d);
float pifdist(int x1, int y1, int x2, int y2);

// Gradients themselves
void surface_radial_gradient       (surface*, color_rgba c1, color_rgba c2, int x1, int y1, float r);
void surface_radial_gradient_points(surface*, color_rgba c1, color_rgba c2, int x1, int y1, int x2, int y2);
void surface_rombical_gradient     (surface*, color_rgba c1, color_rgba c2, int x1, int y1, float r);
void surface_square_gradient       (surface*, color_rgba c1, color_rgba c2, int x1, int y1, float r);
void surface_linear_gradient       (surface*, color_rgba c1, color_rgba c2, int x1, int y1, int x2, int y2, uint8_t symmetry);
void surface_strcrc                (surface*, double cx, double cy, double r, double w, color_rgba c, uint8_t);
void surface_pointy_weird1(surface* obj, double x1, double y1, double x2, double y2);
void surface_pointy_weird2(surface* obj, color_rgba c1, color_rgba c2, color_rgba c3, double x1, double y1, double x2, double y2, double x3, double y3);
