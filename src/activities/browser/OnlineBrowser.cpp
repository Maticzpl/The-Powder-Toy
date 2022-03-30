#include "OnlineBrowser.h"

#include "graphics/FontData.h"
#include "gui/SDLWindow.h"
#include "gui/Static.h"
#include "gui/Appearance.h"
#include "gui/Icons.h"
#include "gui/Spinner.h"
#include "gui/DropDown.h"
#include "gui/IconTextBox.h"
#include "gui/Time.h"
#include "gui/Pagination.h"
#include "language/Language.h"
#include "backend/SearchSaves.h"
#include "backend/SearchTags.h"
#include "backend/GetPTI.h"
#include "backend/Startup.h"
#include "backend/GetSaveInfo.h"
#include "backend/GetSaveData.h"
#include "backend/DeleteSave.h"
#include "backend/PublishSave.h"
#include "backend/FavorSave.h"
#include "backend/Backend.h"
#include "common/Platform.h"
#include "activities/game/Game.h"
#include "activities/taskbatch/TaskBatch.h"
#include "activities/preview/Preview.h"
#include "activities/dialog/Confirm.h"
#include "simulation/GameSave.h"
#include "Button.h"
#include "Metrics.h"

namespace activities::browser
{
	class TagStatic : public gui::Static
	{
		int wingIntrude = 0;

		void UpdateWingIntrude()
		{
			wingIntrude = (Size().x - gui::SDLWindow::Ref().TextSize(Text()).x) / 2 - 10;
		}

	public:
		void Draw() const final override
		{
			Static::Draw();
			auto &g = gui::SDLWindow::Ref();
			auto abs = AbsolutePosition();
			auto size = Size();
			g.DrawLine(abs + gui::Point{          0, FONT_H / 2 + 1 }, abs + gui::Point{          wingIntrude    , FONT_H / 2 + 1 }, textColor);
			g.DrawLine(abs + gui::Point{ size.x - 1, FONT_H / 2 + 1 }, abs + gui::Point{ size.x - wingIntrude - 1, FONT_H / 2 + 1 }, textColor);
		}

		void Text(const String &newText) final override
		{
			Static::Text(newText);
			UpdateWingIntrude();
		}
		const String &Text() const final override
		{
			return Static::Text();
		}

		virtual void Size(gui::Point newSize) final override
		{
			Static::Size(newSize);
			UpdateWingIntrude();
		}
		gui::Point Size() const
		{
			return Static::Size();
		}
	};

	OnlineBrowser::OnlineBrowser()
	{
		searchDomainDrop = EmplaceChild<gui::DropDown>().get();
		searchDomainDrop->Options({
			String::Build(gui::ColorString({ 0xFF, 0xE0, 0xA0 }), gui::IconString(gui::Icons::powder       , { 1, -2 }), gui::ResetString(), " ", "DEFAULT_LS_ONLINEBROWSER_SEARCHMODE_ALL"_Ls()),
			String::Build(gui::ColorString({ 0xC0, 0xA0, 0x40 }), gui::IconString(gui::Icons::folderBody   , { 1, -2 }), gui::StepBackString(),
			              gui::ColorString({ 0xFF, 0xFF, 0xFF }), gui::IconString(gui::Icons::folderOutline, { 1, -2 }), gui::ResetString(), " ", "DEFAULT_LS_ONLINEBROWSER_SEARCHMODE_OWN"_Ls()),
			String::Build(gui::ColorString({ 0xC0, 0xA0, 0x40 }), gui::IconString(gui::Icons::special      , { 1, -2 }), gui::ResetString(), " ", "DEFAULT_LS_ONLINEBROWSER_SEARCHMODE_FAVORITES"_Ls()),
		});
		searchDomainDrop->CenterSelected(false);
		searchDomainDrop->Position({ 4, 4 });
		searchDomainDrop->Size({ 70, 17 });
		searchDomainDrop->Change([this](int value) {
			OnlineSearchStart(false, query, false, 0, true, searchSort, false, backend::SearchDomain(value), false);
		});
		if (!backend::Backend::Ref().User().detailLevel)
		{
			searchDomainDrop->Enabled(false);
		}

		searchSortDrop = EmplaceChild<gui::DropDown>().get();
		searchSortDrop->Options({
			String::Build(gui::ColorString(gui::Appearance::voteDown), gui::IconString(gui::Icons::votesBody   , { 3,  1 }), gui::StepBackString(),
		                  gui::ColorString(gui::Appearance::voteUp  ), gui::IconString(gui::Icons::votesBody   , { 0, -2 }), gui::StepBackString(),
		                  gui::ColorString({ 0xFF, 0xFF, 0xFF }     ), gui::IconString(gui::Icons::votesOutline, { 0, -2 }), gui::ResetString(), " ", "DEFAULT_LS_ONLINEBROWSER_SORTMODE_BYVOTES"_Ls()),
			String::Build(gui::ColorString({ 0xFF, 0x80, 0x80 }     ), gui::IconString(gui::Icons::calendar    , { 0, -1 }), gui::ResetString(), " ", "DEFAULT_LS_ONLINEBROWSER_SORTMODE_BYDATE"_Ls()),
		});
		searchSortDrop->CenterSelected(false);
		searchSortDrop->Position({ windowSize.x - 74, 4 });
		searchSortDrop->Size({ 70, 17 });
		searchSortDrop->Change([this](int value) {
			OnlineSearchStart(false, query, false, 0, true, backend::SearchSort(value), false, searchDomain, false);
		});

		tagsButton = pagination->EmplaceChild<gui::Button>().get();
		tagsButton->Size(pagination->PrevButton()->Size());
		tagsButton->Position(pagination->PrevButton()->Position());
		tagsButton->Text(String::Build(gui::IconString(gui::Icons::tag, { 0, -2 }), " ", "DEFAULT_LS_ONLINEBROWSER_TAGS"_Ls()));
		tagsButton->Click([this]() {
			ShowTagsOnFirstPage(!ShowTagsOnFirstPage());
		});
		tagsButton->Visible(false);
		tagsButton->Stuck(showTagsOnFirstPage);
		tagsButton->ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ "DEFAULT_LS_ONLINEBROWSER_TAGS_TIP"_Ls(), { 0, 0 }, true }));

		topPanel = EmplaceChild<gui::Component>().get();
		topPanel->Position(topArea.pos);
		topPanel->Size(topArea.size);

		tagStatic = topPanel->EmplaceChild<TagStatic>().get();
		tagStatic->Visible(false);
		tagStatic->Align(gui::Alignment::horizCenter | gui::Alignment::vertCenter);
		tagStatic->Text("DEFAULT_LS_ONLINEBROWSER_POPULARTAGS"_Ls());
		tagStatic->Size({ 300, 15 });
		tagStatic->Position({ (topArea.size.x - tagStatic->Size().x) / 2, 15 });
		tagStatic->TextRect({ { 2, 0 }, tagStatic->Size() - gui::Point{ 2, 0 } });
		tagStatic->TextColor(gui::Appearance::commonYellow);

		tagErrorStatic = topPanel->EmplaceChild<gui::Static>().get();
		tagErrorStatic->Size({ windowSize.x, 15 });
		tagErrorStatic->Position((topArea.size - tagErrorStatic->Size()) / 2);
		tagErrorStatic->Visible(false);
		tagErrorStatic->Align(gui::Alignment::horizCenter | gui::Alignment::vertCenter);
		tagErrorStatic->TextColor(gui::Appearance::commonYellow);

		motdStatic = topPanel->EmplaceChild<gui::Button>().get();
		motdStatic->Position({ 0, 0 });
		motdStatic->Size({ topPanel->Size().x, 15 });
		motdStatic->DrawBody(false);
		motdStatic->DrawBorder(false);
		motdStatic->ActiveText(gui::Button::activeTextNone); // * TODO-REDO_UI: Pointer cursor.
		motdStatic->Visible(false);
		motdStatic->Align(gui::Alignment::horizCenter | gui::Alignment::vertCenter);
		motdStatic->Text("DEFAULT_LS_ONLINEBROWSER_MOTD_LOADING"_Ls());
		
		queryBox->PlaceHolder(String::Build("[", "DEFAULT_LS_ONLINEBROWSER_SEARCH"_Ls(), "]"));

		deleteButton = EmplaceChild<gui::Button>().get();
		deleteButton->Visible(false);
		deleteButton->Text(gui::CommonColorString(gui::commonLightRed) + "DEFAULT_LS_BROWSER_DELETE"_Ls());
		deleteButton->Click([this]() {
			DeleteSelected();
		});

		unpublishButton = EmplaceChild<gui::Button>().get();
		unpublishButton->Visible(false);
		unpublishButton->Click([this]() {
			if (willUnpublish)
			{
				UnpublishSelected();
			}
			else
			{
				PublishSelected();
			}
		});

		favorButton = EmplaceChild<gui::Button>().get();
		favorButton->Visible(false);
		favorButton->Click([this]() {
			if (willUnfavor)
			{
				UnfavorSelected();
			}
			else
			{
				FavorSelected();
			}
		});

		saveControls.push_back(deleteButton);
		saveControls.push_back(unpublishButton);
		saveControls.push_back(favorButton);
		saveControls.push_back(clearSelectionButton);
	}

	bool OnlineBrowser::ValidQuery(const String &newQuery) const
	{
		return !newQuery.size() || newQuery.size() > 3;
	}

	void OnlineBrowser::OnlineSearchStart(bool force, String newQuery, bool resetQueryBox, int newPage, bool resetPageBox, backend::SearchSort newSearchSort, bool resetSearchSortDrop, backend::SearchDomain newSearchDomain, bool resetSearchDomainDrop)
	{
		if (!force &&
		    newSearchSort == searchSort &&
		    newSearchDomain == searchDomain &&
		    newPage == pagination->Page() &&
		    newQuery == query)
		{
			return;
		}
		if (!ValidQuery(newQuery) || !pagination->ValidPage(newPage))
		{
			return;
		}
		if (searchSaves) // * TODO-REDO_UI: Not great, discards valid page input if the request is still in flight.
		{
			return;
		}
		searchSort = newSearchSort;
		searchDomain = newSearchDomain;
		if (resetSearchSortDrop)
		{
			searchSortDrop->Value(int(searchSort));
		}
		if (resetSearchDomainDrop)
		{
			searchDomainDrop->Value(int(searchDomain));
		}
		Browser::SearchStart(force, newQuery, resetQueryBox, newPage, resetPageBox);
		for (auto tb : tagButtons)
		{
			topPanel->RemoveChild(tb);
		}
		tagButtons.clear();
		pageOnlyQuery = searchSort == backend::searchSortByVotes && searchDomain == backend::searchDomainAll && !query.size();
		showingFirstPage = pageOnlyQuery && pagination->Page() == 0;
		UpdateSaveControls();
		pagination->PrevButton()->Visible(!showingFirstPage);
		tagsButton->Visible(showingFirstPage);
		motdStatic->Visible(false);
		tagStatic->Visible(false);
		tagErrorStatic->Visible(false);
		searchSaves = std::make_shared<backend::SearchSaves>(query, pageSize * pagination->Page(), pageSize, searchDomain, searchSort);
		searchSaves->Start(searchSaves);
		if (showingFirstPage)
		{
			searchTags = std::make_shared<backend::SearchTags>(0, tagPageSize);
			searchTags->Start(searchTags);
			motdStatic->Visible(true);
		}
	}

	void OnlineBrowser::SearchStart(bool force, String newQuery, bool resetQueryBox, int newPage, bool resetPageBox)
	{
		OnlineSearchStart(force, newQuery, resetQueryBox, newPage, resetPageBox, searchSort, false, searchDomain, false);
	}

	void OnlineBrowser::ShowTagsOnFirstPage(bool newShowTagsOnFirstPage)
	{
		showTagsOnFirstPage = newShowTagsOnFirstPage;
		tagsButton->Stuck(showTagsOnFirstPage);
		ShiftSaveButtons();
	}

	void OnlineBrowser::ShiftSaveButtons()
	{
		auto shifted = showTagsOnFirstPage && showingFirstPage;
		topPanel->Visible(shifted);
		saveButtonsToDisplay.clear();
		auto count = gridSize.x * gridSize.y;
		if (shifted)
		{
			count -= gridSize.x;
		}
		for (auto i = 0; i < int(saveButtons.size()) && i < count; ++i)
		{
			auto *sb = saveButtons[i].get();
			sb->slot = i;
			if (shifted)
			{
				sb->slot += gridSize.x;
			}
			saveButtonsToDisplay.push_back(sb);
		}
		DisplaySaveButtons();
	}

	void OnlineBrowser::SearchSavesFinish()
	{
		searchSpinner->Visible(false);
		if (searchSaves->status)
		{
			{
				auto pageMax = searchSaves->saveCount / pageSize + (searchSaves->saveCount % pageSize ? 1 : 0) - 1;
				if (pageMax < 0)
				{
					pageMax = 0;
				}
				if (pageOnlyQuery)
				{
					pageMax += 1; // * FP is returned as the first page of zero-query searches by the website.
				}
				pagination->PageMax(pageMax);
			}

			auto count = pageSize;
			for (auto i = 0; i < count && i < int(searchSaves->saves.size()); ++i)
			{
				auto &save = searchSaves->saves[i];
				auto *button = EmplaceChild<Button>(save.name, save.username, save.scoreUp, save.scoreDown, !save.published, true).get();
				auto username = save.username;
				button->ContextMenu([this, save, username, button]() {
					std::vector<Button::ContextMenuEntry> entries;
					entries.push_back(Button::ContextMenuEntry{ "DEFAULT_LS_BROWSER_CONTEXT_OPEN"_Ls(), [this, save]() {
						QuickOpen(save);
					} });
					if (button->Selected())
					{
						entries.push_back(Button::ContextMenuEntry{ "DEFAULT_LS_BROWSER_CONTEXT_DESELECT"_Ls(), [this, button]() {
							button->Selected(false);
							UpdateSaveControls();
						} });
					}
					else
					{
						entries.push_back(Button::ContextMenuEntry{ "DEFAULT_LS_BROWSER_CONTEXT_SELECT"_Ls(), [this, button]() {
							button->Selected(true);
							UpdateSaveControls();
						} });
					}
					entries.push_back(Button::ContextMenuEntry{ "DEFAULT_LS_ONLINEBROWSER_CONTEXT_HISTORY"_Ls(), [this, save]() {
						OnlineSearchStart(false, "history:" + save.idv.id, true, 0, true, backend::searchSortByDate, true, backend::searchDomainAll, true);
					} });
					entries.push_back(Button::ContextMenuEntry{ "DEFAULT_LS_ONLINEBROWSER_CONTEXT_RELATED"_Ls(), [this, username]() {
						OnlineSearchStart(false, "user:" + username, true, 0, true, backend::searchSortByVotes, true, backend::searchDomainAll, true);
					} });
					return entries;
				});
				auto lowResGetImage = std::make_shared<backend::GetPTI>(activities::preview::Preview::LowResImagePath(save.idv));
				button->Click([this, save, lowResGetImage]() {
					Preview(save.idv, lowResGetImage);
				});
				button->AltClick([this, save]() {
					QuickOpen(save);
				});
				button->ShowGroup([this, username]() {
					OnlineSearchStart(false, "user:" + username, true, 0, true, backend::searchSortByVotes, true, backend::searchDomainAll, true);
				});
				button->Select([this](bool selected) {
					UpdateSaveControls();
				});
				lowResGetImage->Start(lowResGetImage);
				auto sbi = std::make_unique<SaveButtonInfo>();
				sbi->button = button;
				sbi->save = save;
				sbi->getThumbnail = lowResGetImage;
				saveButtons.push_back(std::move(sbi));
			}
			ShiftSaveButtons();
			if (!searchSaves->saves.size())
			{
				errorStatic->Visible(true);
				errorStatic->Text("DEFAULT_LS_BROWSER_NOSAVES"_Ls());
			}
		}
		else
		{
			errorStatic->Visible(true);
			errorStatic->Text(String::Build("DEFAULT_LS_ONLINEBROWSER_SAVELIST_ERROR"_Ls(), searchSaves->status.error));
			pagination->PageMax(0);
		}
		searchSaves.reset();
	}

	void OnlineBrowser::Preview(backend::SaveIDV idv, std::shared_ptr<backend::GetPTI> getThumbnail)
	{
		auto *preview = gui::SDLWindow::Ref().EmplaceBack<preview::Preview>(idv, getThumbnail).get();
		preview->Success([this]() {
			Quit();
		});
		preview->Deleted([this]() {
			Refresh();
		});
	}

	void OnlineBrowser::QuickOpen(backend::SaveInfo save)
	{
		String repr;
		if (save.idv.version)
		{
			repr = "DEFAULT_LS_ONLINEBROWSER_OPEN_REPR_VERSIONED"_Ls(save.idv.id, gui::Time::FormatAbsolute(save.updatedDate));
		}
		else
		{
			repr = "DEFAULT_LS_ONLINEBROWSER_OPEN_REPR_DEFAULT"_Ls(save.idv.id);
		}
		auto getSaveInfo = std::make_shared<backend::GetSaveInfo>(save.idv);
		getSaveInfo->relativeWeight = 10;
		auto getSaveData = std::make_shared<backend::GetSaveData>(save.idv);
		getSaveData->relativeWeight = 90;
		gui::SDLWindow::Ref().EmplaceBack<taskbatch::TaskBatch>("DEFAULT_LS_ONLINEBROWSER_OPEN_TITLE"_Ls(), std::vector<taskbatch::TaskInfo>{
			taskbatch::TaskInfo{
				getSaveInfo,
				"DEFAULT_LS_ONLINEBROWSER_QUICKOPEN_INFO_PROGRESS"_Ls(repr),
				"DEFAULT_LS_ONLINEBROWSER_QUICKOPEN_INFO_FAILURE"_Ls(repr),
			},
			taskbatch::TaskInfo{
				getSaveData,
				"DEFAULT_LS_ONLINEBROWSER_QUICKOPEN_DATA_PROGRESS"_Ls(repr),
				"DEFAULT_LS_ONLINEBROWSER_QUICKOPEN_DATA_FAILURE"_Ls(repr),
			},
		})->Finish([this, getSaveInfo, getSaveData, repr]() {
			auto errorValid = false;
			String error;
			if (!getSaveInfo->status)
			{
				error = getSaveInfo->status.error;
				errorValid = true;
			}
			else if (!getSaveData->status)
			{
				error = getSaveData->status.error;
				errorValid = true;
			}
			else
			{
				try
				{
					auto saveData = std::make_shared<GameSave>(getSaveData->data);
					saveData->Expand();
					game::Game::Ref().LoadOnlineSave(getSaveInfo->info, saveData);
					Quit();
				}
				catch (const ParseException &e)
				{
					error = ByteString(e.what()).FromUtf8();
					errorValid = true;
				}
			}
			if (errorValid)
			{
				// * TODO-REDO_UI: Display String::Build("Failed to open save ", repr, ": ", error).
			}
		});
	}

	void OnlineBrowser::SearchTagsFinish()
	{
		if (searchTags->status)
		{
			tagStatic->Visible(true);
			auto maxTagVotes = 0;
			if (searchTags->tags.size())
			{
				maxTagVotes = searchTags->tags[0].count;
			}
			auto buttonOffsetY = 30;
			auto buttonAreaSize = topArea.size - gui::Point{ 0, buttonOffsetY };
			auto buttonSize = gui::Point{ buttonAreaSize.x / tagGridSize.x, buttonAreaSize.y / tagGridSize.y };
			for (auto i = 0; i < tagPageSize && i < int(searchTags->tags.size()); ++i)
			{
				auto x = i % tagGridSize.x;
				auto y = i / tagGridSize.x;
				auto text = searchTags->tags[i].text;
				auto *button = topPanel->EmplaceChild<gui::Button>().get();
				button->Size(buttonSize);
				button->TextRect({ { 2, 0 }, buttonSize - gui::Point{ 4, 0 } });
				button->Position({ buttonAreaSize.x * x / tagGridSize.x, buttonAreaSize.y * y / tagGridSize.y + buttonOffsetY });
				button->Click([this, text]() {
					OnlineSearchStart(false, text, true, 0, true, backend::searchSortByVotes, true, backend::searchDomainAll, true);
				});
				button->DrawBorder(false);
				auto intensity = 192;
				if (maxTagVotes)
				{
					intensity = 127 + (128 * searchTags->tags[i].count) / searchTags->tags[0].count;
				}
				button->Text(gui::ColorString({ intensity, intensity, intensity, 0xFF }) + text);
				button->HoverCursor(gui::Cursor::cursorHand);
				tagButtons.push_back(button);
			}
		}
		else
		{
			tagErrorStatic->Visible(true);
			tagErrorStatic->Text(String::Build("DEFAULT_LS_ONLINEBROWSER_TAGLIST_ERROR"_Ls(), searchTags->status.error));
		}
		searchTags.reset();
	}

	void OnlineBrowser::UpdateTasks()
	{
		if (searchSaves && searchSaves->complete)
		{
			SearchSavesFinish();
		}
		if (searchTags && searchTags->complete)
		{
			SearchTagsFinish();
		}
		for (auto &sb : saveButtons)
		{
			auto *osb = static_cast<SaveButtonInfo *>(sb.get());
			if (osb->getThumbnail && osb->getThumbnail->complete)
			{
				if (osb->getThumbnail->status)
				{
					osb->button->ImageData(osb->getThumbnail->image);
				}
				else
				{
					osb->button->Text(String::Build(gui::CommonColorString(gui::commonYellow), osb->getThumbnail->status.shortError));
				}
				osb->getThumbnail.reset();
			}
		}
		if (waitingForMotd && backend::Startup::Ref().Complete())
		{
			waitingForMotd = false;
			if (backend::Startup::Ref().Status())
			{
				auto uri = backend::Startup::Ref().MotdUri();
				motdStatic->Text(backend::Startup::Ref().MotdText());
				// * TODO-REDO_UI: Is this needed?
				// motdStatic->ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ uri, { 0, 0 }, true }));
				if (uri.size())
				{
					motdStatic->HoverCursor(gui::Cursor::cursorHand);
					motdStatic->Click([uri]() {
						Platform::OpenURI(uri.ToUtf8());
					});
				}
			}
			else
			{
				motdStatic->Text(String::Build(gui::ColorString(gui::Appearance::commonYellow), String::Build("DEFAULT_LS_ONLINEBROWSER_MOTD_ERROR"_Ls(), backend::Startup::Ref().Error())));
			}
		}
	}

	void OnlineBrowser::Tick()
	{
		ModalWindow::Tick();
		UpdateTasks();
	}

	void OnlineBrowser::DeleteSelected()
	{
		StartTaskBatchOnSelected(
			"DEFAULT_LS_BROWSER_DELETE_TITLE"_Ls(),
			"DEFAULT_LS_BROWSER_DELETE_BODY"_Ls(),
			[](backend::SaveIDV idv) -> String { return "DEFAULT_LS_BROWSER_DELETE_PROGRESS"_Ls(idv.id); },
			[](backend::SaveIDV idv) -> String { return "DEFAULT_LS_BROWSER_DELETE_FAILURE"_Ls(idv.id); },
			[](backend::SaveIDV idv) -> std::shared_ptr<common::Task> { return std::make_shared<backend::DeleteSave>(idv.id, false); }
		);
	}

	void OnlineBrowser::UnpublishSelected()
	{
		StartTaskBatchOnSelected(
			"DEFAULT_LS_ONLINEBROWSER_UNPUBLISH_TITLE"_Ls(),
			"DEFAULT_LS_ONLINEBROWSER_UNPUBLISH_BODY"_Ls(),
			[](backend::SaveIDV idv) -> String { return "DEFAULT_LS_ONLINEBROWSER_UNPUBLISH_PROGRESS"_Ls(idv.id); },
			[](backend::SaveIDV idv) -> String { return "DEFAULT_LS_ONLINEBROWSER_UNPUBLISH_FAILURE"_Ls(idv.id); },
			[](backend::SaveIDV idv) -> std::shared_ptr<common::Task> { return std::make_shared<backend::DeleteSave>(idv.id, true); }
		);
	}

	void OnlineBrowser::PublishSelected()
	{
		StartTaskBatchOnSelected(
			"DEFAULT_LS_ONLINEBROWSER_PUBLISH_TITLE"_Ls(),
			"DEFAULT_LS_ONLINEBROWSER_PUBLISH_BODY"_Ls(),
			[](backend::SaveIDV idv) -> String { return "DEFAULT_LS_ONLINEBROWSER_PUBLISH_PROGRESS"_Ls(idv.id); },
			[](backend::SaveIDV idv) -> String { return "DEFAULT_LS_ONLINEBROWSER_PUBLISH_FAILURE"_Ls(idv.id); },
			[](backend::SaveIDV idv) -> std::shared_ptr<common::Task> { return std::make_shared<backend::PublishSave>(idv.id); }
		);
	}

	void OnlineBrowser::UnfavorSelected()
	{
		StartTaskBatchOnSelected(
			"DEFAULT_LS_ONLINEBROWSER_UNFAVOR_TITLE"_Ls(),
			"DEFAULT_LS_ONLINEBROWSER_UNFAVOR_BODY"_Ls(),
			[](backend::SaveIDV idv) -> String { return "DEFAULT_LS_ONLINEBROWSER_UNFAVOR_PROGRESS"_Ls(idv.id); },
			[](backend::SaveIDV idv) -> String { return "DEFAULT_LS_ONLINEBROWSER_UNFAVOR_FAILURE"_Ls(idv.id); },
			[](backend::SaveIDV idv) -> std::shared_ptr<common::Task> { return std::make_shared<backend::FavorSave>(idv.id, false); }
		);
	}

	void OnlineBrowser::FavorSelected()
	{
		StartTaskBatchOnSelected(
			"DEFAULT_LS_ONLINEBROWSER_FAVOR_TITLE"_Ls(),
			"DEFAULT_LS_ONLINEBROWSER_FAVOR_BODY"_Ls(),
			[](backend::SaveIDV idv) -> String { return "DEFAULT_LS_ONLINEBROWSER_FAVOR_PROGRESS"_Ls(idv.id); },
			[](backend::SaveIDV idv) -> String { return "DEFAULT_LS_ONLINEBROWSER_FAVOR_FAILURE"_Ls(idv.id); },
			[](backend::SaveIDV idv) -> std::shared_ptr<common::Task> { return std::make_shared<backend::FavorSave>(idv.id, true); }
		);
	}

	void OnlineBrowser::StartTaskBatchOnSelected(
		String titleString,
		String confirmString,
		String (*progressString)(backend::SaveIDV idv),
		String (*failurePrefixString)(backend::SaveIDV idv),
		std::shared_ptr<common::Task> (*createTask)(backend::SaveIDV idv)
	)
	{
		std::vector<taskbatch::TaskInfo> tasks;
		StringBuilder bodyBuilder; // * Arnold Schwarzenegger
		bodyBuilder << confirmString << "\n";
		for (auto *sb : GetSelected())
		{
			auto *osb = static_cast<const SaveButtonInfo *>(sb);
			tasks.push_back(taskbatch::TaskInfo{
				createTask(osb->save.idv),
				progressString(osb->save.idv),
				failurePrefixString(osb->save.idv),
			});
			bodyBuilder << "\n - " << "DEFAULT_LS_ONLINEBROWSER_TASKBATCH_ITEM"_Ls(osb->save.idv.id, osb->save.name);
		}
		gui::SDLWindow::Ref().EmplaceBack<dialog::Confirm>(titleString, bodyBuilder.Build())->Finish([this, tasks, titleString](bool confirmed) {
			if (confirmed)
			{
				gui::SDLWindow::Ref().EmplaceBack<taskbatch::TaskBatch>(titleString, tasks)->Finish([this]() {
					Refresh();
				});
			}
		});
	}

	void OnlineBrowser::UpdateSaveControls()
	{
		auto somethingSelected = false;
		auto user = backend::Backend::Ref().User();
		auto canManage = true;
		auto canFavor = true;
		if (!user.detailLevel)
		{
			canFavor = false;
		}
		auto hasPublic = false;
		auto hasFavorite = searchDomain == backend::searchDomainFavorites; // * TODO-REDO_UI: Get favourite status somehow and decide based on that.
		for (auto *sb : GetSelected())
		{
			auto *osb = static_cast<const SaveButtonInfo *>(sb);
			somethingSelected = true;
			if (!user.detailLevel || (user.elevation == backend::UserInfo::elevationNone && osb->save.username != user.name))
			{
				canManage = false;
			}
			if (osb->save.idv.version)
			{
				canManage = false;
				canFavor = false;
			}
			if (osb->save.published)
			{
				hasPublic = true;
			}
		}
		unpublishButton->Text(
			hasPublic ?
			(gui::CommonColorString(gui::commonLightRed) + "DEFAULT_LS_ONLINEBROWSER_UNPUBLISH"_Ls()) :
			(gui::CommonColorString(gui::commonYellow) + "DEFAULT_LS_ONLINEBROWSER_PUBLISH"_Ls())
		);
		favorButton->Text(
			hasFavorite ?
			"DEFAULT_LS_ONLINEBROWSER_UNFAVOR"_Ls() :
			"DEFAULT_LS_ONLINEBROWSER_FAVOR"_Ls()
		);
		pagination->PageBefore()->Visible(!somethingSelected);
		pagination->PageBox()->Visible(!somethingSelected);
		pagination->PageAfter()->Visible(!somethingSelected);
		deleteButton->Visible(somethingSelected);
		deleteButton->Enabled(canManage);
		unpublishButton->Visible(somethingSelected);
		unpublishButton->Enabled(canManage);
		favorButton->Visible(somethingSelected);
		favorButton->Enabled(canFavor);
		clearSelectionButton->Visible(somethingSelected);
		willUnpublish = hasPublic;
		willUnfavor = hasFavorite;
		Browser::UpdateSaveControls();
	}
}
