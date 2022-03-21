#pragma once
#include "Config.h"

#include "Component.h"

namespace gui
{
	class Button;
	class Static;
	class TextBox;

	class Pagination : public Component
	{
	public:
		using ChangeCallback = std::function<void (int newPage, bool resetPageBox)>;

	private:
		ChangeCallback change;

		int page = 0;
		int pageMax = 0;

		Button *prevButton = nullptr;
		Button *nextButton = nullptr;
		Static *pageBefore = nullptr;
		TextBox *pageBox = nullptr;
		Static *pageAfter = nullptr;

	public:
		Pagination();

		bool ValidPage(int newPage) const
		{
			return newPage >= 0 && newPage <= pageMax;
		}

		void UpdateControls();
		
		void Page(int newPage, bool resetPageBox);
		int Page() const
		{
			return page;
		}
		
		void PageMax(int newPageMax);
		int PageMax() const
		{
			return pageMax;
		}

		Button *PrevButton() const
		{
			return prevButton;
		}

		Button *NextButton() const
		{
			return nextButton;
		}

		Static *PageBefore() const
		{
			return pageBefore;
		}

		TextBox *PageBox() const
		{
			return pageBox;
		}

		Static *PageAfter() const
		{
			return pageAfter;
		}

		void Change(ChangeCallback newChange)
		{
			change = newChange;
		}
		ChangeCallback Change() const
		{
			return change;
		}

		virtual void Size(Point newSize) override;
		Point Size() const
		{
			return Component::Size();
		}
	};
}
