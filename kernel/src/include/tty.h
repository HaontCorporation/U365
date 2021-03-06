#ifndef __TTY_H
#define __TTY_H

#include <stdint.h>

extern int tty_w;
extern int tty_h;
extern uint8_t tty_drawCharBg;
extern int tty_x, tty_y;
extern volatile uint8_t tty_feedback;
extern uint8_t tty_color;
extern uint8_t *bfn_437;
extern uint8_t *tty_buf;

extern uint8_t vgafnt[4096];

void tty_putentryat(uint8_t, int, int, uint8_t);
void setupFonts();
void initTTY();
void tty_putchar(char);
void tty_setcolor(uint8_t);
uint8_t tty_mkcolor(uint8_t, uint8_t);
void tty_wrstr(const char *);
void tty_goToPos(int, int);

#endif