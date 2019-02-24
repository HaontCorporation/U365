#include "arch/i686/kbd.h"
#include "arch/i686/regs.h"
#include "debug.h"
#include "time.h"

#define ARR_LENGTH(a) (a/sizeof(a[0]))

typedef struct {
	uint8_t scancode;
	char chr; //Character it corresponds to.
} kybd_scancode;

static kybd_scancode regular_scancodes[] = {
  /* Numerical keys */
  {0x02, '1'}, {0x03, '2'}, {0x04, '3'}, {0x05, '4'}, {0x06, '5'}, {0x07, '6'}, {0x08, '7'}, {0x09, '8'}, {0x0A, '9'}, {0x0B, '0'},
  /* Some characters after numerical keys */
  {0x0C, '-'}, {0x0D, '='}, {0x0E, '\b'}, {0x0F, '\t'},
  /* Alphabet! */
  {0x10, 'q'}, {0x11, 'w'}, {0x12, 'e'}, {0x13, 'r'}, {0x14, 't'}, {0x15, 'y'}, {0x16, 'u'}, {0x17, 'i'}, {0x18, 'o'}, {0x19, 'p'}, {0x1A, '['}, {0x1B, ']'}, {0x1C, '\n'},
  {0x1E, 'a'}, {0x1F, 's'}, {0x20, 'd'}, {0x21, 'f'}, {0x22, 'g'}, {0x23, 'h'}, {0x24, 'j'}, {0x25, 'k'}, {0x26, 'l'}, {0x27, ';'}, {0x28, '\''}, {0x29, '`'},
  {0x2B, '\\'}, {0x2C, 'z'}, {0x2D, 'x'}, {0x2E, 'c'}, {0x2F, 'v'}, {0x30, 'b'}, {0x31, 'n'}, {0x32, 'm'}, {0x33, ','}, {0x34, '.'}, {0x35, '/'}, {0x37, '*'},
  {0x39, ' '}, {0x47, '7'}, {0x48, '8'}, {0x49, '9'}, {0x4A, '-'},
			   {0x4B, '4'}, {0x4C, '5'}, {0x4D, '6'}, {0x4E, '+'},
			   {0x4F, '1'}, {0x50, '2'}, {0x51, '3'},
			   {0x52, '0'}, {0x53, '.'}, {0x00, '\0'}
};
static kybd_scancode uppercase_scancodes[] = {
  /* Numerical keys */
  {0x02, '1'}, {0x03, '2'}, {0x04, '3'}, {0x05, '4'}, {0x06, '5'}, {0x07, '6'}, {0x08, '7'}, {0x09, '8'}, {0x0A, '9'}, {0x0B, '0'},
  /* Some characters after numerical keys */
  {0x0C, '-'}, {0x0D, '='}, {0x0E, '\b'}, {0x0F, '\t'},
  /* Alphabet! */
  {0x10, 'Q'}, {0x11, 'W'}, {0x12, 'E'}, {0x13, 'R'}, {0x14, 'T'}, {0x15, 'Y'}, {0x16, 'U'}, {0x17, 'I'}, {0x18, 'O'}, {0x19, 'P'}, {0x1A, '['}, {0x1B, ']'}, {0x1C, '\n'},
  {0x1E, 'A'}, {0x1F, 'S'}, {0x20, 'D'}, {0x21, 'F'}, {0x22, 'G'}, {0x23, 'H'}, {0x24, 'J'}, {0x25, 'K'}, {0x26, 'L'}, {0x27, ';'}, {0x28, '\''}, {0x29, '`'},
  {0x2B, '\\'}, {0x2C, 'Z'}, {0x2D, 'X'}, {0x2E, 'C'}, {0x2F, 'V'}, {0x30, 'B'}, {0x31, 'N'}, {0x32, 'M'}, {0x33, ','}, {0x34, '.'}, {0x35, '/'}, {0x37, '*'},
  {0x39, ' '}, {0x47, '7'}, {0x48, '8'}, {0x49, '9'}, {0x4A, '-'},
			   {0x4B, '4'}, {0x4C, '5'}, {0x4D, '6'}, {0x4E, '+'},
			   {0x4F, '1'}, {0x50, '2'}, {0x51, '3'},
			   {0x52, '0'}, {0x53, '.'}, {0x00, '\0'}
};
static kybd_scancode shift_modified_scancodes[] = {
  /* Numerical keys */
  {0x02, '!'}, {0x03, '@'}, {0x04, '#'}, {0x05, '$'}, {0x06, '%'}, {0x07, '^'}, {0x08, '&'}, {0x09, '*'}, {0x0A, '('}, {0x0B, ')'},
  /* Some characters after numerical keys */
  {0x0C, '_'}, {0x0D, '+'}, {0x0E, '\b'}, {0x0F, '\t'},
  /* Alphabet! */
  {0x10, 'Q'}, {0x11, 'W'}, {0x12, 'E'}, {0x13, 'R'}, {0x14, 'T'}, {0x15, 'Y'}, {0x16, 'U'}, {0x17, 'I'}, {0x18, 'O'}, {0x19, 'P'}, {0x1A, '{'}, {0x1B, '}'}, {0x1C, '\n'},
  {0x1E, 'A'}, {0x1F, 'S'}, {0x20, 'D'}, {0x21, 'F'}, {0x22, 'G'}, {0x23, 'H'}, {0x24, 'J'}, {0x25, 'K'}, {0x26, 'L'}, {0x27, ':'}, {0x28, '"'}, {0x29, '~'},
  {0x2B, '\\'}, {0x2C, 'Z'}, {0x2D, 'X'}, {0x2E, 'C'}, {0x2F, 'V'}, {0x30, 'B'}, {0x31, 'N'}, {0x32, 'M'}, {0x33, '<'}, {0x34, '>'}, {0x35, '?'}, {0x37, '*'},
  {0x39, ' '}, {0x47, '7'}, {0x48, '8'}, {0x49, '9'}, {0x4A, '-'},
			   {0x4B, '4'}, {0x4C, '5'}, {0x4D, '6'}, {0x4E, '+'},
			   {0x4F, '1'}, {0x50, '2'}, {0x51, '3'},
			   {0x52, '0'}, {0x53, '.'}, {0x00, '\0'}
};

static kbd_event keyboard_buffer[256] = {};
static uint32_t kybd_buf_in  = 0;
static uint32_t kybd_buf_out = 0;

void kbd_buffer_push(kbd_event value)
{
	keyboard_buffer[(kybd_buf_in++) % 256] = value;
	if(kybd_buf_in > 256 && kybd_buf_out > 256)
	{
		kybd_buf_in -= 256;
		kybd_buf_out -= 256;
	}
	if(kybd_buf_in - kybd_buf_out > 256)
	{
		// Handle buffer overflow here
		// actually, nothing bad happens
	}
}

kbd_event kbd_buffer_pop()
{
	static kbd_event none = {0};
	if(kybd_buf_in <= kybd_buf_out)
		return none;
	return keyboard_buffer[(kybd_buf_out++) % 256];
}

//Interrupt part of the keyboard driver.
volatile bool kbd_irq_fired=false;
void kbd_wait_irq()
{
	//if(kbd_irq_fired){while(kbd_irq_fired);return;}
	while(!kbd_irq_fired);
	kbd_irq_fired=false;
}

char kbd_event_convert(kbd_event e)
{
	if(!e.exists || e.release)
		return 0;

	uint8_t shift = (e.lshift || e.rshift) && !e.capslk;
	uint8_t uppercase = e.capslk && !e.lshift && !e.rshift;

	if(uppercase)
	{
		for(int i=0; uppercase_scancodes[i].scancode != 0x00; i++)
			if(uppercase_scancodes[i].scancode == e.code)
			  return uppercase_scancodes[i].chr;
		return 0;
	}
	else if(shift)
	{
		for(int i=0; shift_modified_scancodes[i].scancode != 0x00; i++)
			if(shift_modified_scancodes[i].scancode == e.code)
			  return shift_modified_scancodes[i].chr;
		return 0;
	}
	else {
		for(int i=0; regular_scancodes[i].scancode != 0x00; i++)
			if(regular_scancodes[i].scancode == e.code)
			  return regular_scancodes[i].chr;
		return 0;
	}
}

void kbd_handler(struct regs *UNUSED(r))
{
	static kbd_event state = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	kbd_irq_fired=true;
	//We need to put every pressed printable key to the buffer.
	uint8_t scancode = inb(KBD_DATA);
	if(scancode > 0x80 && scancode < 0xE0)
	{
		state.release = 1;
		scancode -= 0x80;
	}

	switch(scancode)
	{
		case KEY_LSHIFT: state.lshift = !state.release; break;
		case KEY_RSHIFT: state.rshift = !state.release; break;
		case KEY_LCTRL:  state.lctrl  = !state.release; break;
		case KEY_RCTRL:  state.rctrl  = !state.release; break;
		case 0xE0: state.code |= 0x100;
		//case 0xE1: state.code |= 0x200;
		//case 0xF0: state.release = 1;
		//default:   state.release = 0;
	}

	if(scancode != 0xE0 && scancode != 0xE1)
	{
		state.code += scancode;
		kbd_buffer_push(state);
		state.code = 0;
		state.release = 0;
	}
}

uint8_t kbd_getchar()
{
	char ret=0;
	while(!ret)
	{
		// If the queue is empty, try to wait, rush B otherwise :)
		if(kybd_buf_in == kybd_buf_out)
		{
			sleep(1);
			kbd_wait_irq();
		}
		ret = kbd_event_convert(kbd_buffer_pop());
	}
	if(tty_feedback) tty_putchar(ret);
	return ret;
}

size_t kbd_gets(char *s, size_t lim)
{
	uint8_t c = 0;
	size_t i = 0;
	int tfb = tty_feedback;
	tty_feedback = 0;
	while(true)
	{
		c = kbd_getchar();
		if(c > 0)
		{
			if(c == '\b')
			{
				if(i > 0)
				{
					if(tfb) tty_wrstr("\b \b");
					s[--i]=0;
				}
				continue;
			}
			if(i < lim - 1)
			{
				if(tfb) tty_putchar(c);
				s[i++]=c;
			}
			else
			{
				s[i] = 0;
			}
			if(c == '\n')
			{
				s[i] = 0;
				tty_feedback = tfb;
				return i;
			}
		}
	}
	tty_feedback = tfb;
}
/*
This function is waits for keyboard buffer to empty.
*/
void kbd_waitForBufToEmpty()
{
	char c=inb(0x60);
	while(inb(0x60)==c)
		c=inb(0x60);
}
void kbd_scancodes_setup()
{
}
//Keyboard-powered CPU reset.
void kbd_reset_cpu()
{
	asm("cli");
	uint8_t good = 0x02;
	while (good & 0x02)
		good = inb(0x64);
	outb(0x64, 0xFE);
	tty_wrstr("Keyboard CPU reset failed.");
	asm("hlt");
}
