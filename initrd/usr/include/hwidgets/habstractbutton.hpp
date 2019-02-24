#ifndef HABSTRACTBUTTON_H
#define HABSTRACTBUTTON_H

#include <hwidgets/hwidget.hpp>

class habstractbutton : public hwidget
{
	protected:
		bool active;
		bool hover;
		const char* text;
	public:
                habstractbutton(hwidget* p = 0);
                ~habstractbutton();
                bool onmousedown() override;
                bool onmouseup() override;
                void setText(const char*);
};

#endif // HABSTRACTBUTTON_H
