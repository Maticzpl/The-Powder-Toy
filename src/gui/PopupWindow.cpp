#include "PopupWindow.h"

#include "SDLWindow.h"
#include "Appearance.h"

namespace gui
{
	void PopupWindow::Draw() const
	{
		auto abs = AbsolutePosition();
		auto size = Size();
		auto &c = Appearance::colors.inactive.idle;
		auto &g = SDLWindow::Ref();
		g.DrawRect({ abs, size }, c.body);
		ModalWindow::Draw();
	}

	void PopupWindow::DrawAfterChildren() const
	{
		auto abs = AbsolutePosition();
		auto size = Size();
		auto &c = Appearance::colors.inactive.idle;
		auto &g = SDLWindow::Ref();
		g.DrawRectOutline({ abs, size }, c.border);
		g.DrawRectOutline({ abs - Point{ 1, 1 }, size + Point{ 2, 2 } }, c.shadow);
		ModalWindow::DrawAfterChildren();
	}
}
