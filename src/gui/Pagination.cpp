#include "Pagination.h"

#include "Button.h"
#include "IconTextBox.h"
#include "SDLWindow.h"
#include "language/Language.h"
#include "graphics/FontData.h"

namespace gui
{
	Pagination::Pagination()
	{
		prevButton = EmplaceChild<Button>().get();
		prevButton->Click([this]() {
			if (change)
			{
				change(page - 1, true);
			}
		});
		prevButton->Enabled(false);

		nextButton = EmplaceChild<Button>().get();
		nextButton->Click([this]() {
			if (change)
			{
				change(page + 1, true);
			}
		});
		nextButton->Enabled(false);

		pageBefore = EmplaceChild<Static>().get();
		pageBox = EmplaceChild<TextBox>().get();
		pageBox->Change([this]() {
			// * ToNumber<int> returns 0 if it can't parse the number, and -1 is invalid anyway.
			auto newPage = pageBox->Text().ToNumber<int>(true) - 1;
			if (change)
			{
				change(newPage, false);
			}
		});
		pageBox->Validate([this]() {
			auto newPage = pageBox->Text().ToNumber<int>(true) - 1;
			if (!ValidPage(newPage))
			{
				pageBox->Text(String::Build(page + 1));
			}
		});
		pageBox->Align(Alignment::horizCenter);
		pageAfter = EmplaceChild<Static>().get();

		UpdateControls();
	}

	void Pagination::Page(int newPage, bool resetPageBox)
	{
		page = newPage;
		if (resetPageBox)
		{
			pageBox->Text(String::Build(page + 1));
		}
		UpdateControls();
	}

	void Pagination::PageMax(int newPageMax)
	{
		pageMax = newPageMax;
		UpdateControls();
	}

	void Pagination::UpdateControls()
	{
		prevButton->Enabled(ValidPage(page - 1));
		nextButton->Enabled(ValidPage(page + 1));
		auto pageMaxHuman = pageMax + 1;
		auto before = "DEFAULT_LS_PAGINATION_BEFORE"_Ls(pageMaxHuman);
		auto box = String::Build(pageMaxHuman);
		auto after = "DEFAULT_LS_PAGINATION_AFTER"_Ls(pageMaxHuman);
		auto &g = SDLWindow::Ref();
		auto textBoxBorder = 5;
		auto widthBefore = g.TextSize(before).x;
		auto widthBox = g.TextSize(box).x;
		auto widthAfter = g.TextSize(after).x;
		auto widthFull = widthBefore + widthBox + widthAfter + 4 * textBoxBorder;
		auto left = (Size().x - widthFull) / 2;
		pageBefore->Position({ left, 0 });
		pageBefore->Size({ widthBefore, Size().y });
		pageBefore->Text(before);
		pageBefore->TextRect(pageBefore->TextRect().Offset({ -2, 0 }).Grow(Point{ 4, 0 }));
		pageBox->Position({ left + widthBefore + textBoxBorder, 0 });
		pageBox->Size({ widthBox + 2 * textBoxBorder, Size().y });
		pageBox->TextRect(pageBox->TextRect().Offset({ 2, (Size().y - FONT_H - 5) / 2 }).Grow(Point{ -2, 0 }));
		pageAfter->Position({ left + widthBefore + widthBox + 4 * textBoxBorder, 0 });
		pageAfter->Size({ widthAfter, Size().y });
		pageAfter->Text(after);
		pageAfter->TextRect(pageAfter->TextRect().Offset({ -2, 0 }).Grow(Point{ 4, 0 }));
	}

	void Pagination::Size(Point newSize)
	{
		Component::Size(newSize);
		UpdateControls();
	}
}
