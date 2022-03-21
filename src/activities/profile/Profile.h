#pragma once

#include "gui/PopupWindow.h"

namespace gui
{
	class TextBox;
	class Label;
}

namespace activities::profile
{
	class Profile : public gui::PopupWindow
	{
		gui::Label *websiteTextBox = nullptr;
		gui::TextBox *biographyTextBox = nullptr;

	public:
		Profile(bool editable);
	};
}
