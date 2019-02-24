#include <env.h>
#include <stdio.h>
#include <graphics/gradients.h>

void gradient_test(int argc, char** argv)
{
	const char* names[] = {"radial", "linear", "rombical", "square", "weird", "circle"};
	static int ln = 0;
	set_env_variable("GO", names[ln++]);
	if(ln >= 6)
		ln = 0;

	printf("Gradient!\n");
	if(argc<2)
	{
		fprintf(stderr, "error: no arguments.\n");
		return;
	}

	if(strcmp(argv[1], "radial")==0)
	{
		printf("Selected '%s' test.\n", argv[1]);
		surface* gradient = surface_new();
		surface_default_init(gradient, 200, 200);
		surface_radial_gradient(gradient, color_white, color_blue, 100, 100, 100);
		rect r={400, 30, 200, 200};
		surface_apply(surface_screen, gradient, r);
	}
	if(strcmp(argv[1], "linear")==0)
	{
		printf("Selected '%s' test.\n", argv[1]);
		surface* gradient = surface_new();
		surface_default_init(gradient, 200, 200);
		surface_linear_gradient(gradient, color_yellow, color_red, 0, 0, 50, 150, 0);
		rect r={400, 260, 200, 200};
		surface_apply(surface_screen, gradient, r);
	}

	if(strcmp(argv[1], "rombical")==0)
	{
		printf("Selected '%s' test.\n", argv[1]);
		surface* gradient = surface_new();
		surface_default_init(gradient, 200, 200);
		surface_rombical_gradient(gradient, color_yellow, color_red, 100, 100, 80);
		rect r={630, 30, 200, 200};
		surface_apply(surface_screen, gradient, r);
	}
	if(strcmp(argv[1], "square")==0)
	{
		printf("Selected '%s' test.\n", argv[1]);
		surface* gradient = surface_new();
		surface_default_init(gradient, 200, 200);
		surface_square_gradient(gradient, color_yellow, color_red, 100, 100, 80);
		rect r={630, 260, 200, 200};
		surface_apply(surface_screen, gradient, r);
	}
	if(strcmp(argv[1], "weird")==0)
	{
		printf("Selected '%s' test.\n", argv[1]);
		surface* gradient = surface_new();
		surface_default_init (gradient, 400, 400);
		surface_pointy_weird2(gradient, color_red,   color_green,  color_blue,      0, 300, 100,   0, 350, 250);
		surface_pointy_weird2(gradient, color_green, color_cyan,   color_blue,    100,   0, 400,   0, 350, 250);
		surface_pointy_weird2(gradient, color_red,   color_blue,   color_magenta,   0, 300, 350, 250, 400, 400);
		surface_pointy_weird2(gradient, color_red,   color_yellow, color_green,     0, 300,   0,   0, 100,   0);
		surface_pointy_weird2(gradient, color_blue,  color_cyan,   color_magenta, 350, 250, 400,   0, 400, 400);
		surface_pointy_weird2(gradient, color_black, color_red,    color_magenta,   0, 400,   0, 300, 400, 400);
		rect r={400, 0, 400, 400};
		surface_apply(surface_screen, gradient, r);
	}
	if(strcmp(argv[1], "circle")==0)
	{
		printf("Selected '%s' test.\n", argv[1]);
		surface* result = surface_new();
		surface_default_init(result, 200, 200);
		surface_strcrc(result, 100, 100, 80, 5, color_red, 0);
		rect r={400, 260, 200, 200};
		surface_apply(surface_screen, result, r);
		r.x += 250;
		surface_strcrc(result, 100, 100, 80, 5, color_green, 1);
		surface_apply(surface_screen, result, r);
		surface* res2 = surface_new();
		surface_default_init(res2, 200, 200);
		surface_strcrc(res2, 100, 100, 70, 1, color_white, 1);
		r.y -= 250;
		surface_apply_notransparency(surface_screen, res2, r);
	}
	printf("It was Surface test in U365. Made with Surface.\n");
}
