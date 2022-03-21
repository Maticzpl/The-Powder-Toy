#pragma once
#include "Config.h"

#include "PopupWindow.h"

namespace gui
{
	class Button;

	class ButtonMenu : public PopupWindow
	{
	public:
		using SelectCallback = std::function<void (int)>;

	private:
		SelectCallback select;
		std::vector<String> options;
		std::vector<Button *> buttons;
		Point buttonSize = { 1, 1 };

		void ResizeButtons();

	public:
		ButtonMenu();

		void Select(SelectCallback newSelect)
		{
			select = newSelect;
		}
		SelectCallback Select() const
		{
			return select;
		}

		void Options(const std::vector<String> &newOptions);
		const std::vector<String> &Options() const
		{
			return options;
		}

		void ButtonSize(Point newButtonSize);
		Point ButtonSize() const
		{
			return buttonSize;
		}

		void SensiblePosition(Point newPosition);

		void Cancel() final override;
	};
}
