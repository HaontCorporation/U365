#include "arch/i686/devices.h"

#include <stdint.h>
#include <stdbool.h>

#include "tty_old.h"
#include "sys.h"
#include "debug.h"
#include <libc/stdio.h>
#include <modules.h>
#include <display.h>

#include "arch/i686/io.h"
#include "arch/i686/idt.h"
#include "arch/i686/multitasking.h"

#include "graphics/bmp.h"
#include "graphics/surface.h"

//Thanks for Brandon F. for PIT code
void timer_phase(int hz)
{
	int divisor = 1193180 / hz;       /* Calculate our divisor */
	outb(0x43, 0x36);             /* Set our command byte 0x36 */
	outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
	outb(0x40, divisor >> 8);     /* Set high byte of divisor */
}
volatile uint64_t timer_ticks;
bool cursor_displayed=false;
void timer_handler(regs* r __attribute__((unused)))
{
	//tty_wrstr("Timer interrupt!\n");
	timer_ticks++; //Just increment timer_ticks.
	//pit_do_multitasking_work(r);
    return;
    if(__displays[__display_current].ready && !(timer_ticks % 10) /* each 10 ticks */)
    {
        rect r = {0, 0, surface_screen->w, surface_screen->h};
        surface_apply_notransparency(surface_screen, __displays[__display_current].target, r);
    }
}

void sleep(int ticks)
{
	unsigned int eticks;

	eticks = timer_ticks + ticks;
	while(timer_ticks < eticks)
	{
		__asm__ __volatile__ ("sti//hlt//cli");
	}
}
uint64_t get_current_pit_time()
{
	return timer_ticks;
}
void setupPIT()
{
	timer_phase(1000);
	irq_install_handler(0, timer_handler);
}

uint8_t mouse_ready = 0;
#ifdef MODULE_MOUSE
int32_t mouse_x  = 0;
int32_t mouse_y  = 0;
uint8_t mouse_b1 = 0;
uint8_t mouse_b2 = 0;
uint8_t mouse_b3 = 0;
uint8_t mouse_b4 = 0;
uint8_t mouse_b5 = 0;
int     mouse_wheel = 0; // change to 0 after each reading
//Mouse functions

color_rgba ums[32][32];

surface* cursor_new = 0;
//This shouldn't be used outside of mouse driver.
typedef struct mouse_flags_byte
{
	unsigned int   left_button : 1;
	unsigned int  right_button : 1;
	unsigned int middle_button : 1;
	unsigned int       always1 : 1;
	unsigned int        x_sign : 1;
	unsigned int        y_sign : 1;
	unsigned int    x_overflow : 1;
	unsigned int    y_overflow : 1;
} __attribute__((packed)) mouse_flags_byte;
extern struct dev_ps2m_mouse_packet {
    int16_t movement_x;
    int16_t movement_y;
    uint8_t button_l;
    uint8_t button_m;
    uint8_t button_r;
} ps2m_buffer;

void mouse_parse_packet(const char* buf, uint8_t has_wheel, uint8_t has_5_buttons)
{
	mouse_flags_byte* mfb = (mouse_flags_byte*)(buf);
	if(mfb->x_overflow || mfb->y_overflow || !mfb->always1)
		return;
	int offx = (int16_t)(0xff00 * mfb->x_sign) | buf[1];
	int offy = (int16_t)(0xff00 * mfb->y_sign) | buf[2];
	mouse_x += offx;
	mouse_y -= offy;
	mouse_b1 = mfb->left_button;
	mouse_b2 = mfb->right_button;
	mouse_b3 = mfb->middle_button;
    ps2m_buffer.movement_x = offx;
    ps2m_buffer.movement_y = offy;
    ps2m_buffer.button_l = mouse_b1;
    ps2m_buffer.button_r = mouse_b2;
    ps2m_buffer.button_m = mouse_b3;


	if(has_wheel)
	{
		// parse wheel (byte 3, bits 0-3)
		mouse_wheel += (char)((!!(buf[3] & 0x8)) * 0xf8 | (buf[3] & 0x7));
		if(has_5_buttons)
		{
			mouse_b4 = !!(buf[3] & 0x10);
			mouse_b4 = !!(buf[3] & 0x20);
			// parse buttons 4-5 (byte 3, bits 4-5)
		}
	}
}

void mouse_erase()
{
	for(int i = 0; i < 32; ++i)
	{
		for(int j = 0; j < 32; ++j)
		{
			surface_set_pixel(surface_screen, ums[i][j], mouse_x + j - 1, mouse_y + i - 1);
		}
	}
}

void mouse_draw()
{

	for(int i = 0; i < 32; i++)
	{
		for(int j = 0; j < 32; j++)
		{
			ums[i][j] = surface_get_pixel(surface_screen, mouse_x + j - 1, mouse_y + i - 1);
		}
	}

	if(cursor_new)
	{
		rect r = {mouse_x, mouse_y, cursor_new->w, cursor_new->h};
		surface_apply_notransparency(surface_screen, cursor_new, r);
	}
	else
	{
		for(int y = 0; y < 20; y++)
			for(int x = 0; x < y + 1 && x + y * 2 < 40; x++)
			{
				color_rgba pix=surface_get_pixel(surface_screen, mouse_x+x, mouse_y+y);
				pix.r=255-pix.r;
				pix.g=255-pix.g;
				pix.b=255-pix.b;
				surface_set_pixel(surface_screen, pix, mouse_x+x, mouse_y+y);
			}
	}
}
void mouse_handler(regs* UNUSED(a_r)) //struct regs *a_r (not used but just there)
{
	static uint8_t punymutex = 0;
	if(punymutex)
		return;
	punymutex = 1;

	uint8_t status=inb(0x64);
	if((status & 1) == 0 || (status >> 5 & 1) == 0)
		return;

	static int recbyte = 0;
	static char mousebuf[5];

	mousebuf[recbyte++] = inb(0x60);
	if(recbyte == 3 /* + has_wheel */)
	{

		recbyte = 0;

		static uint8_t drawn = 0;
		if(drawn)
		{
			mouse_erase();
		}
		drawn = 1;

		mouse_parse_packet(mousebuf, 0, 0);

		// Bounds
		if(mouse_x < 0)
			mouse_x = 0;
		if(mouse_y < 0)
			mouse_y = 0;
		if(mouse_x > (int)(surface_screen->w))
			mouse_x=surface_screen->w-10;
		if(mouse_y > (int)(surface_screen->h))
            mouse_y=surface_screen->h-10;

		// Redraw the cursor
		mouse_draw();
	}

	punymutex = 0;
}

void mouse_wait(uint8_t a_type) //unsigned char
{
	uint32_t _time_out=100000; //unsigned int
	if(a_type==0)
	{
		while(_time_out--) //Data
		{
			if((inb(0x64) & 1)==1)
			{
				return;
			}
		}
		return;
	}
	else
	{
		while(_time_out--) //Signal
		{
			if((inb(0x64) & 2)==0)
			{
				return;
			}
		}
		return;
	}
}

void mouse_write(uint8_t a_write) //unsigned char
{
	//Wait to be able to send a command
	mouse_wait(1);
	//Tell the mouse we are sending a command
	outb(0x64, 0xD4);
	//Wait for the final part
	mouse_wait(1);
	//Finally write
	outb(0x60, a_write);
}

uint8_t mouse_read()
{
	//Gets response from mouse
	mouse_wait(0);
	return inb(0x60);
}

void mouse_install()
{
	/*for(int i=0; i<8; i++)
		for(int j=0; j<5; j++)
			pixels[i][j]=getPix(mx+j,mouse_y+i);*/
	uint8_t _status;  //unsigned char
	//Enable the auxiliary mouse device
	mouse_wait(1);
	outb(0x64, 0xA8);
	//Enable the interrupts
	mouse_wait(1);
	outb(0x64, 0x20);
	mouse_wait(0);
	_status=(inb(0x60) | 2);
	mouse_wait(1);
	outb(0x64, 0x60);
	mouse_wait(1);
	outb(0x60, _status);
	//Tell the mouse to use default settings
	mouse_write(0xF6);
	mouse_read();  //Acknowledge
	//Enable the mouse
	mouse_write(0xF4);
	mouse_read();  //Acknowledge

	//Set cursor coordinates in middle of the screen.
	mouse_x = surface_screen->w / 2;
	mouse_y = surface_screen->h / 2;
	//Setup mouse IRQ handler. We are doing it before mouse initalization, so first interrupt will be catched anyway.
	irq_install_handler(12, mouse_handler);
	mouse_ready = 1;
}
#endif // MODULE_MOUSE

void initKbdInt()
{
	void kbd_handler(struct regs *);
	irq_install_handler(1,kbd_handler);
}

