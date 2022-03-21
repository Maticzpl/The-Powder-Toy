#include "DropDown.h"

#include "ButtonMenu.h"
#include "SDLWindow.h"

#include <SDL.h>

namespace gui
{
	void DropDown::HandleClick(int button)
	{
		Button::HandleClick(button);
		if (button == SDL_BUTTON_LEFT)
		{
			if (options.size())
			{
				auto *bm = SDLWindow::Ref().EmplaceBack<ButtonMenu>().get();
				bm->Options(options);
				bm->ButtonSize(Size());
				auto pos = AbsolutePosition();
				if (centerSelected)
				{
					pos.y -= (Size().y - 1) * value;
				}
				bm->SensiblePosition(pos);
				bm->Select([this](int index) {
					Value(index);
					TriggerChange();
				});
			}
		}
	}

	void DropDown::TriggerChange()
	{
		if (change)
		{
			change(value);
		}
	}

	void DropDown::OptionToText()
	{
		// * The order if these two if-blocks matters. We'd rather have the value
		//   become -1 than 0 when there are no options in the dropdown.
		if (value < 0)
		{
			value = 0;
		}
		if (value > int(options.size()) - 1)
		{
			value = int(options.size()) - 1;
		}
		Text(value >= 0 ? options[value] : String());
	}

	void DropDown::Value(int newValue)
	{
		value = newValue;
		OptionToText();
	}

	void DropDown::Options(const std::vector<String> &newOptions)
	{
		options = newOptions;
		OptionToText();
	}
}
