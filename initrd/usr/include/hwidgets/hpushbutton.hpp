#ifndef HPUSHBUTTON_H
#define HPUSHBUTTON_H

#include <hwidgets/habstractbutton.hpp>

class hpushbutton : public habstractbutton
{
	public:
                hpushbutton(hwidget* p = 0);
                ~hpushbutton();
		void ondraw() override;
};

#endif // HPUSHBUTTON_H
