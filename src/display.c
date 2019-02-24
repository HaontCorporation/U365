#include <display.h>
#include <graphics/surface.h>
display __displays[8];
int __display_current = 0;
surface* current_surface;
void display_set_id(int id)
{
    __display_current = id;
}
int display_get_id()
{
    return __display_current;
}
void displays_init()
{
    for(int i=0; i<8; i++)
    {
        __displays[i].ready = 1;
        __displays[i].target = surface_new();
        surface_default_init(__displays[i].target, surface_screen->w, surface_screen->h);
    }
    __display_current = 0;
    current_surface = __displays[0].target;
    rect r = {0, 0, surface_screen->w, surface_screen->h};
    surface_fill_rect(current_surface, color_black, r);
}
