#include <stdint.h>
void tui_draw_box(uint8_t color, int x, int y, int w, int h);
void tui_draw_text(uint8_t color, int x, int y, const char* text);
void tui_bordered_box(uint8_t, int, int, int, int);
void tui_fill_area(uint8_t, int, int, int, int, uint8_t);
void tui_draw_lim_text(uint8_t, int, int, int, int, const char*, unsigned);
