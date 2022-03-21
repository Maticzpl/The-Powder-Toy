#pragma once
#include "Config.h"

#include "gui/PopupWindow.h"
#include "common/String.h"

namespace gui
{
	class Button;
	class Label;
	class IconTextBox;
}

namespace activities::login
{
	class Login : public gui::PopupWindow
	{
		gui::IconTextBox *usernameBox = nullptr;
		gui::IconTextBox *passwordBox = nullptr;

	public:
		Login();

		bool KeyPress(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt) final override;
	};
}
