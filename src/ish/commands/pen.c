#include <stdio.h>
#include <graphics/surface.h>
#include <graphics/gradients.h>
#include <arch/i686/devices.h>
#include <modules.h>

int pen_main()
{
	surface* target_surface = surface_screen;
	unsigned int width, height;
	printf("Welcome to PEN - a simple drawing program.\n");
#ifdef MODULE_MOUSE
	printf("Use your mouse to draw.\nPress ESC to exit\n");

	width  = 200;
	height = 200;
	surface* sf   = surface_new();
	surface* grad = surface_new();
	surface_default_init(sf,   width, height);
	surface_default_init(grad, 10,    256);

	rect img_pos     = {(target_surface->w-width) / 2, (target_surface->h-height) / 2, width, height};
	rect img_border  = {img_pos.x - 5, img_pos.y - 5, img_pos.w + 10, img_pos.h + 10};
	rect scale_pos   = {target_surface->w - 40, target_surface->h / 2 - 128, 10, 256};
	rect all_surface = {0, 0, width, height};
	rect active_pos  = {target_surface->w - 40, target_surface->h / 2 - 168, 30, 30};

	surface_fill_rect(target_surface, color_light_gray, img_border);
	surface_fill_rect(sf, color_white, all_surface);
	img_border.x = target_surface->w - 45;
	img_border.y = surface_screen->h / 2 - 173;
	img_border.w = 40;
	img_border.h = 306;
	surface_fill_rect(surface_screen, color_light_gray, img_border);

	surface_linear_gradient(grad, color_red, color_black, 0, 0, 0, 256, 0);
	surface_apply_notransparency(surface_screen, grad, scale_pos);
	scale_pos.x += 10;
	surface_linear_gradient(grad, color_green, color_black, 0, 0, 0, 256, 0);
	surface_apply_notransparency(surface_screen, grad, scale_pos);
	scale_pos.x += 10;
	surface_linear_gradient(grad, color_blue, color_black, 0, 0, 0, 256, 0);
	surface_apply_notransparency(surface_screen, grad, scale_pos);

	color_rgba active = color_black;

	surface_apply_notransparency(surface_screen, sf, img_pos);
	surface_fill_rect(surface_screen, active, active_pos);

	uint8_t hold = 0;
	int sx, sy, hx = 0, hy = 0;

	while(1)
	{
		kbd_event e = kbd_buffer_pop();
		if(e.exists)
		{
			if(e.code == KEY_ESC)
				break;
		}

		if(mouse_b1)
		{
			sx = mouse_x;
			sy = mouse_y;
			if(
					sx >= (int)(img_pos.x) &&
					sy >= (int)(img_pos.y) &&
					sx <  (int)(img_pos.x + img_pos.w) &&
					sy <  (int)(img_pos.y + img_pos.h))
			{
				if(hold)
				{
					rect r = {hx - img_pos.x, hy - img_pos.y, sx - hx, sy - hy};
					surface_put_line(sf, r, active);
				}
				else
				{
					surface_set_pixel(sf, active, sx - img_pos.x, sy - img_pos.y);
					hold = 1;
				}
				hx = sx;
				hy = sy;
			}
			else
				hold = 0;

			if(sy >= (int)(surface_screen->h / 2 - 128) && sy < (int)(surface_screen->h / 2 + 128))
			{
				if(sx >= (int)(surface_screen->w - 40) && sx < (int)(surface_screen->w - 30))
					active.r = (int)(surface_screen->h / 2 + 128) - sy;
				if(sx >= (int)(surface_screen->w - 30) && sx < (int)(surface_screen->w - 20))
					active.g = (int)(surface_screen->h / 2 + 128) - sy;
				if(sx >= (int)(surface_screen->w - 20) && sx < (int)(surface_screen->w - 10))
					active.b = (int)(surface_screen->h / 2 + 128) - sy;
			}
			surface_apply_notransparency(surface_screen, sf, img_pos);
			surface_fill_rect(surface_screen, active, active_pos);
		}
		else
		{
			hold = 0;
		}
	}
	putchar('\n');
	return 0;
#else
	printf("This program requires MODULE_MOUSE\n");
	return -1;
#endif // MODULE_MOUSE
}
