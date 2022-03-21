#pragma once
#include "Config.h"

#include "Button.h"

#include <functional>

namespace gui
{
	class DropDown : public Button
	{
	public:
		using ChangeCallback = std::function<void (int)>;

	private:
		ChangeCallback change;
		int value = 0;
		bool centerSelected = true;
		std::vector<String> options;
		void OptionToText();

	public:
		void HandleClick(int button) final override;

		void Change(ChangeCallback newChange)
		{
			change = newChange;
		}
		ChangeCallback Change() const
		{
			return change;
		}

		void Value(int newValue);
		int Value() const
		{
			return value;
		}

		void Options(const std::vector<String> &newOptions);
		const std::vector<String> &Options() const
		{
			return options;
		}

		void CenterSelected(bool newCenterSelected)
		{
			centerSelected = newCenterSelected;
		}
		bool CenterSelected() const
		{
			return centerSelected;
		}

		void TriggerChange();
	};
}
