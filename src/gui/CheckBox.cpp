#include "CheckBox.h"

#include "SDLWindow.h"
#include "Appearance.h"

#include <SDL.h>

namespace gui
{
	void CheckBox::Draw() const
	{
		auto abs = AbsolutePosition();
		auto &cc = Appearance::colors.inactive;
		auto &c = Enabled() ? (UnderMouse() ? cc.hover : cc.idle) : cc.disabled;
		auto &tc = UnderMouse() ? cc.hover : cc.idle;
		auto &g = SDLWindow::Ref();
		g.DrawRect({ abs + Point{ 0, 1 }, { 12, 12 } }, c.body);
		g.DrawRectOutline({ abs + Point{ 0, 1 }, { 12, 12 } }, c.border);
		if (value)
		{
			g.DrawRect({ abs + Point{ 3, 4 }, { 6, 6 } }, c.border);
		}
		g.DrawText(abs + drawTextAt, text, tc.text, Enabled() ? 0 : SDLWindow::drawTextDarken);
	}

	bool CheckBox::MouseDown(Point current, int button)
	{
		return true;
	}

	void CheckBox::MouseClick(Point current, int button, int clicks)
	{
		if (button == SDL_BUTTON_LEFT)
		{
			value = !value;
			if (change)
			{
				change(value);
			}
		}
	}

	void CheckBox::Size(Point newSize)
	{
		Static::Size(newSize);
		TextRect(TextRect().Rebase({ 14, 0 }));
	}
}
