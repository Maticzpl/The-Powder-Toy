#include "IconTextBox.h"

#include "SDLWindow.h"
#include "Appearance.h"

namespace gui
{
	void IconTextBox::Size(Point newSize)
	{
		Component::Size(newSize);
		TextRect(Rect({ { iconPadding, 0 }, Size() - Point{ iconPadding, 0 } }));
	}

	void IconTextBox::Draw() const
	{
		auto &cc = Appearance::colors.inactive;
		auto &c = UnderMouse() ? cc.hover : cc.idle;
		TextBox::Draw();
		SDLWindow::Ref().DrawText(AbsolutePosition() + Point{ 3, 3 }, iconString, c.text, Enabled() ? 0 : SDLWindow::drawTextDarken);
	}

	void IconTextBox::IconPadding(int newIconPadding)
	{
		iconPadding = newIconPadding;
		Size(Size());
	}
}
