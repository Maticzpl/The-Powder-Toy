#pragma once
#include "Config.h"

#include "TextBox.h"

namespace gui
{
	class IconTextBox : public TextBox
	{
		int iconPadding = 0;
		String iconString;

	public:
		void Size(Point newSize) final override;
		Point Size() const
		{
			return Component::Size();
		}

		void Draw() const final override;

		void IconString(String newIconString)
		{
			iconString = newIconString;
		}
		const String &IconString() const
		{
			return iconString;
		}

		void IconPadding(int newIconPadding);
		const int IconPadding() const
		{
			return iconPadding;
		}
	};
}
