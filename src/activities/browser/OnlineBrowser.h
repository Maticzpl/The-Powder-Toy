#pragma once
#include "Config.h"

#include "Browser.h"
#include "backend/SearchDomain.h"
#include "backend/SearchSort.h"
#include "backend/SaveInfo.h"

#include <memory>
#include <vector>

namespace gui
{
	class DropDown;
}

namespace common
{
	struct Task;
}

namespace backend
{
	class SearchTags;
	class SearchSaves;
	class GetPTI;
}

namespace activities::browser
{
	class TagStatic;

	class OnlineBrowser : public Browser
	{
		struct SaveButtonInfo : Browser::SaveButtonInfo
		{
			backend::SaveInfo save;
			std::shared_ptr<backend::GetPTI> getThumbnail;
		};

		bool showTagsOnFirstPage = true;
		bool waitingForMotd = true;
		bool pageOnlyQuery;
		bool showingFirstPage;

		bool willUnpublish;
		bool willUnfavor;
		gui::Button *deleteButton = nullptr;
		gui::Button *unpublishButton = nullptr;
		gui::Button *favorButton = nullptr;

		void DeleteSelected();
		void UnpublishSelected();
		void PublishSelected();
		void UnfavorSelected();
		void FavorSelected();
		void StartTaskBatchOnSelected(
			String titleString,
			String confirmString,
			String (*progressString)(backend::SaveIDV idv),
			String (*failurePrefixString)(backend::SaveIDV idv),
			std::shared_ptr<common::Task> (*createTask)(backend::SaveIDV idv)
		);

		gui::Button *tagsButton = nullptr;
		gui::DropDown *searchDomainDrop = nullptr;
		gui::DropDown *searchSortDrop = nullptr;
		backend::SearchDomain searchDomain = backend::searchDomainAll;
		backend::SearchSort searchSort = backend::searchSortByVotes;

		TagStatic *tagStatic = nullptr;
		gui::Button *motdStatic = nullptr;
		gui::Static *tagErrorStatic = nullptr;
		gui::Component *topPanel;
		std::vector<gui::Button *> tagButtons;

		std::shared_ptr<backend::SearchTags> searchTags;
		void SearchTagsFinish();

		std::shared_ptr<backend::SearchSaves> searchSaves;
		void SearchSavesFinish();

		void UpdateTasks();
		void ShiftSaveButtons();

		void QuickOpen(backend::SaveInfo save);
		void Preview(backend::SaveIDV idv, std::shared_ptr<backend::GetPTI> getThumbnail);

		void OnlineSearchStart(bool force, String newQuery, bool resetQueryBox, int newPage, bool resetPageBox, backend::SearchSort newSearchSort, bool resetSearchSortDrop, backend::SearchDomain newSearchDomain, bool resetSearchDomainDrop);
		void SearchStart(bool force, String newQuery, bool resetQueryBox, int newPage, bool resetPageBox) final override;

		bool ValidQuery(const String &newQuery) const;

		void ShowTagsOnFirstPage(bool newShowTagsOnFirstPage);
		bool ShowTagsOnFirstPage() const
		{
			return showTagsOnFirstPage;
		}

		void UpdateSaveControls() final override;

	public:
		OnlineBrowser();

		void Tick() final override;
	};
}
