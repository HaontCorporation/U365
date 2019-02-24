#include <hwidgets/habstractbutton.hpp>
#include <memory.h>
#include <stdio.h>

habstractbutton::habstractbutton(hwidget* p) : hwidget(p) {}
habstractbutton::~habstractbutton(){}

bool habstractbutton::onmousedown()
{
    active = 1;
    return 1;
}

bool habstractbutton::onmouseup()
{
    active = 0;
    return 1;
}

void habstractbutton::setText(const char* nt)
{
    text = nt;
}
