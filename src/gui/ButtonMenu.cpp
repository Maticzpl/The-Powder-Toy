#include "ButtonMenu.h"

#include "Button.h"
#include "Rect.h"

namespace gui
{
	// * TODO-REDO_UI: Maybe make single-click selection possible somehow?

	ButtonMenu::ButtonMenu()
	{
	}

	void ButtonMenu::ResizeButtons()
	{
		auto counter = 0;
		for (const auto &button : buttons)
		{
			button->Size(buttonSize);
			button->Position({ 0, (buttonSize.y - 1) * counter });
			counter += 1;
		}
		Size({ buttonSize.x, (buttonSize.y - 1) * counter + 1 });
	}

	void ButtonMenu::Options(const std::vector<String> &newOptions)
	{
		options = newOptions;
		for (const auto &button : buttons)
		{
			RemoveChild(button);
		}
		buttons.clear();
		auto counter = 0;
		for (const auto &option : options)
		{
			auto *button = EmplaceChild<Button>().get();
			button->Text(option);
			button->Click([this, counter]() {
				Quit();
				if (select)
				{
					select(counter);
				}
			});
			buttons.push_back(button);
			counter += 1;
		}
		ResizeButtons();
	}

	void ButtonMenu::ButtonSize(Point newButtonSize)
	{
		buttonSize = newButtonSize;
		ResizeButtons();
	}

	void ButtonMenu::Cancel()
	{
		Quit();
	}

	void ButtonMenu::SensiblePosition(Point newPosition)
	{
		Position(Rect{ { 0, 0 }, Point{ WINDOWW, WINDOWH } - Size() }.Clamp(newPosition));
	}
}
