#pragma once
#include "Config.h"

#include "Browser.h"
#include "common/Path.h"

class GameSave;

namespace gui
{
	class DropDown;
	class IconTextBox;
}

namespace graphics
{
	class ThumbnailRendererTask;
}

namespace activities::browser
{
	class SearchSavesTask;

	class LocalBrowser : public Browser
	{
		struct SaveButtonInfo : Browser::SaveButtonInfo
		{
			std::shared_ptr<graphics::ThumbnailRendererTask> saveRender;
			String displayName;
			String path;
		};

		enum SearchSort
		{
			searchSortByName,
			searchSortByDate,
			searchSortMax, // * Must be the last enum constant.
		};
		SearchSort searchSort = searchSortByName;
		gui::DropDown *searchSortDrop = nullptr;
		gui::Button *foldersButton = nullptr;

		void LocalSearchStart(
			bool force,
			String newQuery,
			bool resetQueryBox,
			int newPage,
			bool resetPageBox,
			SearchSort newSearchSort,
			bool resetSearchSortDrop
		);
		void SearchStart(bool force, String newQuery, bool resetQueryBox, int newPage, bool resetPageBox) final override;

		std::shared_ptr<SearchSavesTask> searchSaves;
		void SearchSavesFinish();

		gui::Button *deleteButton = nullptr;
		void UpdateSaveControls() final override;

		void UpdateTasks();

		void DeleteSelected();
		void Open(String displayName, std::shared_ptr<GameSave> save);

		Path searchRoot;
		void NavigateTo(Path newSearchRoot);

	public:
		LocalBrowser();

		void Tick() final override;
	};
}
