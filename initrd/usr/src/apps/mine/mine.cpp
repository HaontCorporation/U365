#include <stdio.h>
#include <surface.h>
#include <math.h>
#include <memory.h>
#include <hwidgets/hpushbutton.hpp>
#include <hwidgets/hcheckbox.hpp>

int main()
{
	printf("This was supposed to be a minesweeper, but it isn't, so what ever\n");

	hwidget* box  = new hwidget;
	hpushbutton* butt = new hpushbutton(box);
	hpushbutton* butu = new hpushbutton(box);
	hcheckbox* chbox = new hcheckbox(box);
	box->resize(200, 100);
	butt->resize(100, 20);
	butu->resize(100, 20);
	chbox->resize(100, 20);
	butt->setPosition(50, 10);
	butu->setPosition(50, 40);
	chbox->setPosition(50, 70);
	butt->setText("Hello?");
	chbox->state = 1;
	
	box->draw();

	rect r = {0, 0, 200, 100};

	surface_screen_apply(box->getSurface(), r);
	return 0;
}
