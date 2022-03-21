#include "Separator.h"

#include "SDLWindow.h"
#include "Appearance.h"

namespace gui
{
	void Separator::Draw() const
	{
		SDLWindow::Ref().DrawRectOutline({ AbsolutePosition(), Size() }, Appearance::colors.inactive.idle.border);
	}
}
