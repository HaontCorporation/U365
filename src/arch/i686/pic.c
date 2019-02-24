#include "../../../include/arch/i686/io.h"

void init_pics(int pic1, int pic2)
{
    outb(PIC1, ICW1);
    outb(PIC2, ICW1);
    outb(PIC1 + 1, pic1);
    outb(PIC2 + 1, pic2);
    outb(PIC1 + 1, 4);
    outb(PIC2 + 1, 2);
    outb(PIC1 + 1, ICW4);
    outb(PIC2 + 1, ICW4);
    outb(PIC1 + 1, 0xFF);
}
