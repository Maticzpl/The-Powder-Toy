#include "Button.h"

#include "SDLWindow.h"
#include "Appearance.h"
#include "ModalWindow.h"

#include <SDL.h>

namespace gui
{
	Button::Button()
	{
		TextColor({ 0, 0, 0, 0 });
		Align(Alignment::horizCenter | Alignment::vertCenter);
	}

	void Button::Draw() const
	{
		auto abs = AbsolutePosition();
		auto size = Size();
		auto active = (stuck || Dragging(SDL_BUTTON_LEFT));
		auto &cc = active ? Appearance::colors.active : Appearance::colors.inactive;
		auto &c = Enabled() ? (UnderMouse() ? cc.hover : cc.idle) : cc.disabled;
		auto &tc = UnderMouse() ? cc.hover : cc.idle;
		auto &g = SDLWindow::Ref();
		if (drawBody)
		{
			g.DrawRect({ abs, size }, bodyColor.a ? bodyColor : c.body);
		}
		if (drawBorder)
		{
			g.DrawRectOutline({ abs, size }, c.border);
		}
		uint32_t dtFlags = 0;
		if (active)
		{
			switch (activeTextMode)
			{
			case activeTextInverted:
				dtFlags = SDLWindow::drawTextInvert;
				break;

			case activeTextDarkened:
				dtFlags = SDLWindow::drawTextDarken;
				break;

			default:
				break;
			}
		}
		if (!Enabled())
		{
			switch (disabledTextMode)
			{
			case disabledTextDarkened:
				dtFlags = SDLWindow::drawTextDarken;
				break;

			default:
				break;
			}
		}
		g.DrawText(abs + drawTextAt, textToDraw, textColor.a ? textColor : tc.text, dtFlags);
	}

	bool Button::MouseDown(Point current, int button)
	{
		return true;
	}

	void Button::MouseClick(Point current, int button, int clicks)
	{
		HandleClick(button);
	}

	bool Button::KeyPress(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt)
	{
		return false;
	}

	bool Button::KeyRelease(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt)
	{
		return false;
	}

	void Button::HandleClick(int button)
	{
		if (button == SDL_BUTTON_LEFT)
		{
			if (SDLWindow::Ref().ModCtrl() && altClick)
			{
				TriggerAltClick();
			}
			else
			{
				TriggerClick();
			}
		}
	}

	void Button::TriggerClick()
	{
		if (click)
		{
			click();
		}
	}

	void Button::TriggerAltClick()
	{
		if (altClick)
		{
			altClick();
		}
	}
}
