#ifndef HCHECKBOX_H
#define HCHECKBOX_H

#include <hwidgets/habstractbutton.hpp>

class hcheckbox : public habstractbutton
{
	public:
                hcheckbox(hwidget* p = 0);
                ~hcheckbox();
		
		uint8_t state;
		
		void ondraw() override;
		virtual void checked();
};

#endif // HCHECKBOX_H
