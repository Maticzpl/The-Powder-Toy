#pragma once
#include "Config.h"

#include "gui/PopupWindow.h"
#include "common/String.h"

namespace gui
{
	class Static;
}

namespace activities::dialog
{
	class Dialog : public gui::PopupWindow
	{
	protected:
		gui::Point targetSize = { 0, 0 };
		gui::Static *title = nullptr;

		Dialog(String titleText, String bodyText, gui::Point windowSize);
	};
}
