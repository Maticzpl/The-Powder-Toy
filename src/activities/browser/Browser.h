#pragma once
#include "Config.h"

#include "gui/ModalWindow.h"
#include "common/String.h"

#include <memory>
#include <vector>

namespace gui
{
	class Button;
	class Static;
	class TextBox;
	class IconTextBox;
	class Pagination;
	class Spinner;
}

namespace activities::browser
{
	class Button;

	class Browser : public gui::ModalWindow
	{
	protected:
		virtual void SearchStart(bool force, String newQuery, bool resetQueryBox, int newPage, bool resetPageBox);

		gui::Pagination *pagination = nullptr;

		String query;
		gui::Button *clearQueryButton = nullptr;
		gui::IconTextBox *queryBox = nullptr;
		gui::Static *errorStatic = nullptr;
		gui::Spinner *searchSpinner = nullptr;

		struct SaveButtonInfo
		{
			Button *button = nullptr;
			int slot = 0;

			virtual ~SaveButtonInfo() = default;
		};
		std::vector<std::unique_ptr<SaveButtonInfo>> saveButtons;
		std::vector<SaveButtonInfo *> saveButtonsToDisplay;
		void DisplaySaveButtons();

		std::vector<const SaveButtonInfo *> GetSelected() const;

		gui::Button *clearSelectionButton = nullptr;
		std::vector<gui::Button *> saveControls;
		virtual void UpdateSaveControls();

		Browser();

	public:
		virtual ~Browser();

		bool MouseWheel(gui::Point current, int distance, int direction) final override;

		void Refresh();
	};
}
