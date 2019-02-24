#ifndef DISPLAY_H
#define DISPLAY_H

#include <graphics/surface.h>

typedef struct display display;

struct display
{
	uint8_t ready;
	surface* target;
	void(*kbd_callback)(); // a function that will be called on each KBD IRQ
//	queue kbd_queue;       // a real keyboar buffer that should be rechecked on each IRQ and/or program request
};

extern display __displays[8];
extern int __display_current;
extern surface* current_surface;

void display_set_id(int);
int  display_get_id();
void displays_init(); // This function should

#endif // DISPLAY_H
