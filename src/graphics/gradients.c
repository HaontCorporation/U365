#include <graphics/gradients.h>
#include <math.h>
#include <compare.h>
#include <stdio.h>

color_rgba transcolor(color_rgba c1, color_rgba c2, float d)
{
	if(d < 0) d = 0;
	if(d > 1) d = 1;
	// Maybe some type conversions are needed
	color_rgba result = {
	c1.r * (1 - d) + c2.r * d,
	c1.g * (1 - d) + c2.g * d,
	c1.b * (1 - d) + c2.b * d,
	c1.a * (1 - d) + c2.a * d};
	return result;
}

// Replace it with another function if exists
float pifdist(int x1, int y1, int x2, int y2)
{
	float x = x1 - x2;
	float y = y1 - y2;
	return sqrt(x * x + y * y);
}

void surface_radial_gradient(surface* obj, color_rgba c1, color_rgba c2, int x1, int y1, float r)
{
	for(uint32_t y = 0; y < obj->h; ++y)
	{
		for(uint32_t x = 0; x < obj->w; ++x)
		{
			surface_set_pixel(obj, transcolor(c1, c2, pifdist(x1, y1, x, y) / r), x, y);
		}
	}
}

void surface_radial_gradient_points(surface* obj, color_rgba c1, color_rgba c2, int x1, int y1, int x2, int y2)
{
	surface_radial_gradient(obj, c1, c2, x1, x2, pifdist(x1, y1, x2, y2));
}

void surface_rombical_gradient(surface* obj, color_rgba c1, color_rgba c2, int x1, int y1, float r)
{
	for(uint32_t y = 0; y < obj->h; ++y)
	{
		for(uint32_t x = 0; x < obj->w; ++x)
		{
			int dx = x1 - x, dy = y1 - y;
			dx = abs(dx);
			dy = abs(dy);
			surface_set_pixel(obj, transcolor(c1, c2, (dx + dy) / r), x, y);
		}
	}
}

void surface_square_gradient(surface* obj, color_rgba c1, color_rgba c2, int x1, int y1, float r)
{
	for(uint32_t y = 0; y < obj->h; ++y)
	{
		for(uint32_t x = 0; x < obj->w; ++x)
		{
			int dx = x1 - x, dy = y1 - y;
			dx = abs(dx);
			dy = abs(dy);
			surface_set_pixel(obj, transcolor(c1, c2, max(dx, dy) / r), x, y);
		}
	}
}

void surface_linear_gradient(surface* obj, color_rgba c1, color_rgba c2, int x1, int y1, int x2, int y2, uint8_t symmetry)
{
	int dx = x2 - x1, dy = y2 - y1;
	float dl = sqrt(dx * dx + dy * dy);
	for(uint32_t y = 0; y < obj->h; ++y)
	{
		for(uint32_t x = 0; x < obj->w; ++x)
		{
			if(obj->data[y] == (void*)(0xfdd0))
				printf("Pixel put: %ix%i 0x%08x\n", x, y, (size_t)(obj->data[y]) + x * obj->bpp);
			// Make a vector first
			int ldx = x - x1, ldy = y - y1;
			// Now, make a projection
			float quo = (dx * ldx + dy * ldy) / dl / dl;
			int pdx = dx * quo, pdy = dy * quo;
			float d = 0;
			if(symmetry || (sign(pdx) == sign(dx) && sign(pdy) == sign(dy)))
			{
				float pdl = sqrt(pdx * pdx + pdy * pdy);
				d = pdl / dl;
			}
			surface_set_pixel(obj, transcolor(c1, c2, d), x, y);
		}
	}
}

void surface_pointy_weird1(surface* obj, double x1, double y1, double x2, double y2)
{
	double d1 = pifdist(x1, y1, x2, y2);
	for(uint32_t y = 0; y < obj->h; ++y)
	{
		for(uint32_t x = 0; x < obj->w; ++x)
		{
			double d2 = pifdist(x1, y1, x, y);
			double d3 = pifdist(x2, y2, x, y);
			double d4 = d2 - d3;
			d4 = abs(d4) / d1;
			color_rgba c = {255, 0, 0, 255};
			if(d4 > 1)
				c = color_green;
			else
				c.g = 255 * (1 - d4);
			if((x == x1 && y == y1) || (x == x2 && y == y2))
				c = color_blue;
			surface_set_pixel(obj, c, x, y);
		}
	}
}

void surface_pointy_weird2(surface* obj, color_rgba c1, color_rgba c2, color_rgba c3, double x1, double y1, double x2, double y2, double x3, double y3)
{
	double l12 = pifdist(x1, y1, x2, y2);
	double l23 = pifdist(x2, y2, x3, y3);
	double l31 = pifdist(x3, y3, x1, y1);

	double p = (l12 + l23 + l31) / 2;
	double s = sqrt(p * (p - l12) * (p - l23) * (p - l31));

	double b12 = x2 - x1;
	double b23 = x3 - x2;
	double b31 = x1 - x3;
	double a12 = y1 - y2;
	double a23 = y2 - y3;
	double a31 = y3 - y1;
	double c12 = -(a12 * x2 + b12 * y2);
	double c23 = -(a23 * x2 + b23 * y2);
	double c31 = -(a31 * x1 + b31 * y1);

	c23 = c23;
	c31 = c31;

	for(uint32_t y = 0; y < obj->h; ++y)
	{
		for(uint32_t x = 0; x < obj->w; ++x)
		{
			double d12 = x * a12 + y * b12 + c12;
			double d23 = x * a23 + y * b23 + c23;
			double d31 = x * a31 + y * b31 + c31;

			if(d12 < 0 || d23 < 0 || d31 < 0)
				continue;

			double d1 = pifdist(x1, y1, x, y);
			double d2 = pifdist(x2, y2, x, y);
			double d3 = pifdist(x3, y3, x, y);

			double p12 = (l12 + d1 + d2) / 2;
			double p23 = (l23 + d2 + d3) / 2;
			double p31 = (l31 + d3 + d1) / 2;

			double s12 = sqrt(p12 * (p12 - l12) * (p12 - d1) * (p12 - d2));
			double s23 = sqrt(p23 * (p23 - l23) * (p23 - d2) * (p23 - d3));
			double s31 = sqrt(p31 * (p31 - l31) * (p31 - d3) * (p31 - d1));

			d1 = s23 / s;
			d2 = s31 / s;
			d3 = s12 / s;

			color_rgba c = {
				c1.r * d1 + c2.r * d2 + c3.r * d3,
				c1.g * d1 + c2.g * d2 + c3.g * d3,
				c1.b * d1 + c2.b * d2 + c3.b * d3,
				c1.a * d1 + c2.a * d2 + c3.a * d3
				};
			surface_set_pixel(obj, c, x, y);
		}
	}
}

// Not a macro!!! Optimization motherfucker
inline double sq(double x)
{
	return x * x;
}

void surface_strcrc(surface* obj, double cx, double cy, double r, double w, color_rgba c, uint8_t soft)
{
	uint32_t sx = max(floor(cx - r - w), 0);
	uint32_t sy = max(floor(cy - r - w), 0);
	uint32_t ex = min( ceil(cx + r + w), (double)(obj->w));
	uint32_t ey = min( ceil(cy + r + w), (double)(obj->h));
	w /= 2;

	for(uint32_t y = sy; y <= ey; ++y)
	{
		for(uint32_t x = sx; x <= ex; ++x)
		{
			double pd = sqrt(sq(cx - x) + sq(cy - y));
			// filling may be added by setting each pixel closer than r

			pd -= r;
			pd = abs(pd);

			if(pd < w + 0.5)
			{
				if(pd < w - 0.5)
				{
					surface_apply_pixel(obj, c, x, y); // change to mixed
				}
				else
				{
					if(soft)
					{
						if(y == 40)
							printf("%i\n", (int)((pd - w)*100));
						//c.r = 255;
						color_rgba srcc = surface_get_pixel(obj, x, y);
						surface_set_pixel(obj, transcolor(c, srcc, (pd - w)), x, y);
					}
				}
			}
		}
	}
}
