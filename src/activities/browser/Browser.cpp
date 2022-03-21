#include "Browser.h"

#include "gui/IconTextBox.h"
#include "gui/Button.h"
#include "gui/Icons.h"
#include "gui/SDLWindow.h"
#include "gui/Appearance.h"
#include "gui/Pagination.h"
#include "gui/Spinner.h"
#include "language/Language.h"
#include "Button.h"
#include "Metrics.h"

namespace activities::browser
{
	Browser::Browser()
	{
		Size(windowSize);

		pagination = EmplaceChild<gui::Pagination>().get();
		pagination->Position({ 4, windowSize.y - 21 });
		pagination->Size({ windowSize.x - 8, 17 });
		pagination->Change([this](int newPage, bool resetPageBox) {
			SearchStart(false, query, false, newPage, resetPageBox);
		});
		pagination->ToFrontUnderMouse(false);
		{
			auto *prevButton = pagination->PrevButton();
			auto *nextButton = pagination->NextButton();
			prevButton->Text(String::Build(gui::IconString(gui::Icons::leftArrow, { 0, -1 }), " ", "DEFAULT_LS_BROWSER_PREV"_Ls()));
			prevButton->Size({ 70, pagination->Size().y });
			prevButton->Position({ 0, 0 });
			nextButton->Text(String::Build("DEFAULT_LS_BROWSER_NEXT"_Ls(), " ", gui::IconString(gui::Icons::rightArrow, { 0, -1 })));
			nextButton->Size({ 70, pagination->Size().y });
			nextButton->Position({ pagination->Size().x - nextButton->Size().x, 0 });
		}

		queryBox = EmplaceChild<gui::IconTextBox>().get();
		queryBox->IconString(gui::ColorString({ 0x60, 0x60, 0xFF }) + gui::IconString(gui::Icons::searchBody   , { 1, -2 }) + gui::StepBackString() +
		                     gui::ColorString({ 0xFF, 0xFF, 0xFF }) + gui::IconString(gui::Icons::searchOutline, { 1, -2 }));
		queryBox->IconPadding(16);
		queryBox->Position({ 78, 4 });
		queryBox->Size({ windowSize.x - 172, 17 });
		queryBox->Change([this]() {
			SearchStart(false, queryBox->Text(), false, 0, true);
		});

		clearQueryButton = EmplaceChild<gui::Button>().get();
		clearQueryButton->Position({ windowSize.x - 95, 4 });
		clearQueryButton->Text(gui::IconString(gui::Icons::cross, { 0, -1 }));
		clearQueryButton->Size({ 17, 17 });
		clearQueryButton->Click([this]() {
			SearchStart(false, "", true, 0, true);
			queryBox->Focus();
		});

		clearSelectionButton = EmplaceChild<gui::Button>().get();
		clearSelectionButton->Visible(false);
		clearSelectionButton->Text("DEFAULT_LS_BROWSER_CLEARSELECTION"_Ls());
		clearSelectionButton->Click([this]() {
			for (auto &sb : saveButtons)
			{
				sb->button->Selected(false);
			}
			UpdateSaveControls();
		});

		errorStatic = EmplaceChild<gui::Static>().get();
		errorStatic->Size({ windowSize.x, 15 });
		errorStatic->Position(saveArea.pos + (saveArea.size - errorStatic->Size()) / 2);
		errorStatic->Visible(false);
		errorStatic->Align(gui::Alignment::horizCenter | gui::Alignment::vertCenter);
		errorStatic->TextColor(gui::Appearance::commonYellow);

		searchSpinner = EmplaceChild<gui::Spinner>().get();
		searchSpinner->Size(spinnerSize);
		searchSpinner->Position(saveArea.pos + (saveArea.size - spinnerSize) / 2);
		searchSpinner->Visible(false);

		queryBox->Focus();
	}

	Browser::~Browser()
	{
	}

	void Browser::Refresh()
	{
		SearchStart(true, query, true, pagination->Page(), true);
	}

	void Browser::SearchStart(bool force, String newQuery, bool resetQueryBox, int newPage, bool resetPageBox)
	{
		query = newQuery;
		pagination->Page(newPage, resetPageBox);
		if (resetQueryBox)
		{
			queryBox->Text(query);
		}
		saveButtonsToDisplay.clear();
		for (auto &sb : saveButtons)
		{
			RemoveChild(sb->button);
		}
		saveButtons.clear();
		errorStatic->Visible(false);
		searchSpinner->Visible(true);
	}

	bool Browser::MouseWheel(gui::Point current, int distance, int direction)
	{
		if (direction == 1 && distance > 0)
		{
			pagination->PrevButton()->TriggerClick();
			return true;
		}
		if (direction == 1 && distance < 0)
		{
			pagination->NextButton()->TriggerClick();
			return true;
		}
		return false;
	}

	void Browser::UpdateSaveControls()
	{
		auto controlPadding = 4;
		auto width = controlSize.x * int(saveControls.size()) + controlPadding * (int(saveControls.size()) - 1);
		auto left = (windowSize.x - width) / 2;
		for (auto i = 0; i < int(saveControls.size()); ++i)
		{
			saveControls[i]->Size(controlSize);
			saveControls[i]->Position({ left + i * (controlSize.x + controlPadding), windowSize.y - 21 });
		}
	}

	std::vector<const Browser::SaveButtonInfo *> Browser::GetSelected() const
	{
		std::vector<const Browser::SaveButtonInfo *> selected;
		for (auto &sb : saveButtons)
		{
			if (sb->button->Selected())
			{
				selected.push_back(sb.get());
			}
		}
		return selected;
	}

	void Browser::DisplaySaveButtons()
	{
		for (auto &sb : saveButtons)
		{
			sb->button->Visible(false);
		}
		auto buttonSize = gui::Point{
			(saveArea.size.x - (gridSize.x - 1) * saveSpacing.x) / gridSize.x,
			(saveArea.size.y - (gridSize.y - 1) * saveSpacing.y) / gridSize.y
		};
		auto topLeftToBottomRight = saveArea.size - buttonSize;
		for (auto *sb : saveButtonsToDisplay)
		{
			auto x = sb->slot % gridSize.x;
			auto y = sb->slot / gridSize.x;
			sb->button->Size(buttonSize);
			sb->button->Position(saveArea.pos + gui::Point{ topLeftToBottomRight.x * x / (gridSize.x - 1), topLeftToBottomRight.y * y / (gridSize.y - 1) });
			sb->button->Visible(true);
		}
	}
}
