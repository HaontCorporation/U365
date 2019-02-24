#ifndef HWIDGET_H
#define HWIDGET_H

#include <surface.h>
#include <extra/vector.h>
#include <vector>
typedef struct colorscheme colorscheme;

typedef enum
{
	HW_BORDER    = 1,
	HW_CLOSE     = 2,
	HW_RESIZABLE = 4,
	HW_MAXIMIZE  = 8,
	HW_MINIMIZE  = 16,
	HW_MAXIMIZED = 32
} window_flags;

class hwidget
{
	protected:
		hwidget* parent;                 // Widget with no parent should be a window, right?
		std::vector<hwidget*>  children; // Subwidgets
		surface* surf;                   // Widget's own surface used to render it, also used to store it's actual size
		window_flags wf;

	public:
		hwidget(hwidget* p = 0);
		virtual ~hwidget();
		
		colorscheme* cs;
		bool enabled;
                
                // Modifiers
                void resize(uint32_t w, uint32_t h);
                void setPosition(uint32_t x, uint32_t y);
                void setParent(hwidget*);

                // Getters
                int getWidth();
                int getHeight();
		const surface* getSurface();
		
		// Transmitters
		void draw();
		bool click    (int x, int y, int button);
		bool mouseover(int x, int y);
		bool mouseout (int x, int y);
		bool mousedown(int x, int y, int button);
		bool mouseup  (int x, int y, int button);
                bool focus();

		// Handlers (Like Qt's slots actually)
		virtual void ondraw();
		virtual bool onclick();
		virtual bool onmouseover();
		virtual bool onmouseout();
		virtual bool onmousedown();
		virtual bool onmouseup();
		virtual bool onfocus();
		virtual void onunfocus();
                virtual void onresize(uint32_t, uint32_t);
                
                // Parameters for autosizing
                uint32_t minimumWidth;
                uint32_t maximumWidth;
                uint32_t minimumHeight;
                uint32_t maximumHeight;
};

struct colorscheme
{
	// Main window background color
	color_rgba window_bgcolor;
	// Button background colors
	color_rgba button_bgcolor;
	color_rgba button_bgcolor_hover;
	color_rgba button_bgcolor_active;
	// Button text colors
	color_rgba button_fgcolor;
	color_rgba button_fgcolor_hover;
	color_rgba button_fgcolor_active;
	// Any interactive widget borders
	color_rgba border;
	color_rgba border_hover;
	color_rgba border_active;
	void* fontdata;
};

extern colorscheme colorscheme_default;

#endif // HWIDGET_H
