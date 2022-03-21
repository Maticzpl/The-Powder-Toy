#include "Preview.h"

#include "gui/Separator.h"
#include "gui/Spinner.h"
#include "gui/ImageButton.h"
#include "gui/TextBox.h"
#include "gui/Texture.h"
#include "gui/ScrollPanel.h"
#include "gui/SDLWindow.h"
#include "gui/Icons.h"
#include "gui/Appearance.h"
#include "gui/Pagination.h"
#include "gui/Label.h"
#include "gui/Time.h"
#include "graphics/FontData.h"
#include "backend/Backend.h"
#include "backend/GetPTI.h"
#include "backend/GetSaveData.h"
#include "backend/GetSaveInfo.h"
#include "backend/GetComments.h"
#include "backend/FavorSave.h"
#include "backend/DeleteSave.h"
#include "activities/profile/AvatarButton.h"
#include "activities/browser/Button.h"
#include "activities/browser/VoteBars.h"
#include "activities/dialog/Confirm.h"
#include "activities/game/Game.h"
#include "simulation/GameSave.h"
#include "common/Platform.h"
#include "graphics/ThumbnailRenderer.h"
#include "simulation/ElementGraphics.h"
#include "language/Language.h"

#include <SDL.h>

namespace activities::preview
{
	constexpr auto thumbnailSize     = gui::Point{ XRES, YRES } / 2;
	constexpr auto parentSize        = gui::Point{ WINDOWW, WINDOWH };
	constexpr auto commentsWidth     = 210;
	constexpr auto windowSize        = thumbnailSize + gui::Point{ commentsWidth, 150 };
	constexpr auto spinnerSize       = gui::Point{ 30, 30 };
	constexpr auto rightColumnWidth  = 100;
	constexpr auto avatarSize        = 42;
	constexpr auto commentAvatarSize = 26;
	constexpr auto barsPos           = thumbnailSize + gui::Point{ -browser::scaleVoteBarsTo - 5, 8 };
	constexpr auto commentsOnPage    = 20;

	class IDButton : public gui::Button
	{
		String id;

		void NormalToolTip()
		{
			ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ "DEFAULT_LS_PREVIEW_SAVEID_TIP_NORMAL"_Ls(), { 0, 0 }, true }));
		}

		void ActiveToolTip()
		{
			ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ gui::CommonColorString(gui::commonYellow) + "DEFAULT_LS_PREVIEW_SAVEID_TIP_CLICKED"_Ls(), { 0, 0 }, true }));
		}

	public:
		IDButton(String newId) : id(newId)
		{
			DrawBody(false);
			DrawBorder(false);
			ActiveText(gui::Button::activeTextNone);
			Text(String::Build("DEFAULT_LS_PREVIEW_SAVEID_PREFIX"_Ls(), ": ", id));
			auto &c = gui::Appearance::commonLightGray;
			TextColor({ c.r, c.g, c.b, 255 });
			NormalToolTip();
		}

		void MouseLeave() final override
		{
			NormalToolTip();
		}

		void HandleClick(int button) final override
		{
			if (button == SDL_BUTTON_LEFT)
			{
				gui::SDLWindow::ClipboardText(id);
				ActiveToolTip();
			}
		}
	};

	Preview::Preview(backend::SaveIDV newIdv, std::shared_ptr<backend::GetPTI> inheritedGetThumbnail) : idv(newIdv)
	{
		Size(windowSize);
		Position((parentSize - windowSize) / 2);

		commentPagination = EmplaceChild<gui::Pagination>().get();
		commentPagination->Position({ thumbnailSize.x + 5, windowSize.y - 36 });
		commentPagination->Size({ commentsWidth - 9, 15 });
		commentPagination->Change([this](int newPage, bool resetPageBox) {
			CommentsStart(newPage, resetPageBox);
		});
		{
			auto *prevButton = commentPagination->PrevButton();
			auto *nextButton = commentPagination->NextButton();
			prevButton->Text(String::Build(gui::IconString(gui::Icons::leftArrow, { -1, -1 })));
			prevButton->Size({ 21, commentPagination->Size().y });
			prevButton->Position({ 0, 0 });
			nextButton->Text(gui::IconString(gui::Icons::rightArrow, { 0, -1 }));
			nextButton->Size({ 21, commentPagination->Size().y });
			nextButton->Position({ commentPagination->Size().x - nextButton->Size().x, 0 });
		}

		{
			auto *separator = EmplaceChild<gui::Separator>().get();
			separator->Position({ thumbnailSize.x + 1, 1 });
			separator->Size({ 1, windowSize.y - 2 });
		}
		{
			auto *separator = EmplaceChild<gui::Separator>().get();
			separator->Position({ 1, thumbnailSize.y + 1 });
			separator->Size({ thumbnailSize.x, 1 });
		}

		preview = EmplaceChild<gui::ImageButton>().get();
		preview->Position({ 1, 1 });
		preview->Size(thumbnailSize);
		preview->ToFrontUnderMouse(false);

		previewSpinner = preview->EmplaceChild<gui::Spinner>().get();
		previewSpinner->Position((thumbnailSize - spinnerSize) / 2 + preview->Position());
		previewSpinner->Size(spinnerSize);

		commentPanel = EmplaceChild<gui::ScrollPanel>().get();
		commentPanel->Position({ thumbnailSize.x + 2, 1 });
		commentPanel->Size({ commentsWidth - 3, windowSize.y - 40 });
		commentPanel->Visible(false);
		commentPanel->UnderScroll([this](int) {
			commentPagination->PrevButton()->TriggerClick();
		});
		commentPanel->OverScroll([this](int) {
			commentPagination->NextButton()->TriggerClick();
		});

		{
			auto *separator = EmplaceChild<gui::Separator>().get();
			separator->Position({ thumbnailSize.x + 2, windowSize.y - 39 });
			separator->Size({ commentsWidth - 3, 1 });

			commentSpinner = EmplaceChild<gui::Spinner>().get();
			commentSpinner->Position(commentPanel->Position() + (separator->Position() + separator->Size() - commentPanel->Position() - spinnerSize) / 2);
			commentSpinner->Size(spinnerSize);
		}

		commentBox = EmplaceChild<gui::TextBox>().get();
		commentBox->Position({ thumbnailSize.x + 5, windowSize.y - 19 });
		commentBox->Size({ commentsWidth - 48, 17 });
		commentBox->PlaceHolder(String::Build("[", "DEFAULT_LS_PREVIEW_COMMENT"_Ls(), "]"));

		{
			auto *submit = EmplaceChild<gui::Button>().get();
			submit->Size({ 40, 19 });
			submit->Position(windowSize - submit->Size());
			submit->Text("DEFAULT_LS_PREVIEW_SUBMIT"_Ls());

			open = EmplaceChild<gui::Button>().get();
			open->Size({ 60, 19 });
			open->Position({ 0, windowSize.y - 19 });
			open->Text(String::Build(gui::IconString(gui::Icons::open, { 0, -2 }), " ", "DEFAULT_LS_PREVIEW_OPEN"_Ls()));
			open->Enabled(false);

			favor = EmplaceChild<gui::Button>().get();
			favor->Size({ 60, 19 });
			favor->Position({ 59, windowSize.y - 19 });
			favor->Text(String::Build(gui::ColorString({ 0xC0, 0xA0, 0x40 }), gui::IconString(gui::Icons::special, { 1, -2 }), gui::ResetString(), " ", "DEFAULT_LS_PREVIEW_FAVOR"_Ls()));
			favor->Enabled(false);
			favor->Click([this]() {
				favor->Enabled(false);
				favorSave = std::make_shared<backend::FavorSave>(idv.id, !favor->Stuck());
				favorSave->Start(favorSave);
			});

			auto *report = EmplaceChild<gui::Button>().get();
			report->Size({ 60, 19 });
			report->Position({ 118, windowSize.y - 19 });
			report->Text(String::Build(gui::CommonColorString(gui::commonYellow), gui::IconString(gui::Icons::warning, { 1, -2 }), gui::ResetString(), " ", "DEFAULT_LS_PREVIEW_REPORT"_Ls()));

			auto *browse = EmplaceChild<gui::Button>().get();
			browse->Size({ 116, 19 });
			browse->Position({ 177, windowSize.y - 19 });
			browse->Text(String::Build(gui::IconString(gui::Icons::open, { 0, -2 }), " ", "DEFAULT_LS_PREVIEW_BROWSE"_Ls()));
			browse->Click([this]() {
				Platform::OpenURI(String::Build(SCHEME SERVER "/Browse/View.html?ID=", idv.id).ToUtf8());
			});

			auto *others = EmplaceChild<gui::Button>().get();
			others->Size({ 16, 19 });
			others->Position({ 292, windowSize.y - 19 });
			others->Text("...");
			others->Click([this]() {
				enum Action
				{
					actionHistory,
					actionThisUser,
					actionUnpublish,
					actionDelete,
				};
				std::vector<String> options;
				std::vector<Action> actions;
				options.push_back("DEFAULT_LS_ONLINEBROWSER_CONTEXT_HISTORY"_Ls());
				actions.push_back(actionHistory);
				options.push_back("DEFAULT_LS_ONLINEBROWSER_CONTEXT_RELATED"_Ls());
				actions.push_back(actionThisUser);
				if (canManage)
				{
					// * TODO-REDO_UI: add these to online browser save button context menu
					options.push_back(
						canUnpublish ?
						(gui::CommonColorString(gui::commonLightRed) + "DEFAULT_LS_ONLINEBROWSER_UNPUBLISH"_Ls()) :
						(gui::CommonColorString(gui::commonYellow) + "DEFAULT_LS_ONLINEBROWSER_PUBLISH"_Ls())
					);
					actions.push_back(actionUnpublish);
					options.push_back(gui::CommonColorString(gui::commonLightRed) + "DEFAULT_LS_BROWSER_DELETE"_Ls());
					actions.push_back(actionDelete);
				}
				auto *bm = gui::SDLWindow::Ref().EmplaceBack<gui::ButtonMenu>().get();
				bm->Options(options);
				bm->ButtonSize({ 110, 16 });
				bm->SensiblePosition(gui::SDLWindow::Ref().MousePosition());
				bm->Select([this, actions](int index) {
					switch (actions[index])
					{
					case actionHistory:
						// * TODO-REDO_UI
						break;

					case actionThisUser:
						// * TODO-REDO_UI
						break;

					case actionUnpublish:
						// * TODO-REDO_UI
						break;

					case actionDelete:
						gui::SDLWindow::Ref().EmplaceBack<dialog::Confirm>("DEFAULT_LS_PREVIEW_DELETE_TITLE"_Ls(), "DEFAULT_LS_PREVIEW_DELETE_BODY"_Ls())->Finish([this](bool confirmed) {
							if (confirmed)
							{
								deleteSave = std::make_shared<backend::DeleteSave>(idv.id, false);
								deleteSave->Start(deleteSave);
							}
						});
						break;
					}
				});
			});
		}

		if (inheritedGetThumbnail)
		{
			getThumbnail = inheritedGetThumbnail;
		}
		else
		{
			getThumbnail = std::make_shared<backend::GetPTI>(LowResImagePath(idv));
			getThumbnail->Start(getThumbnail);
		}
		getSaveData = std::make_shared<backend::GetSaveData>(idv);
		getSaveData->Start(getSaveData);
		SaveInfoStart();
		RefreshComments();
		UpdateTasks();
	}

	String Preview::LowResImagePath(backend::SaveIDV idv)
	{
		StringBuilder pathBuilder;
		pathBuilder << "/" << idv.id;
		if (idv.version)
		{
			pathBuilder << "_" << idv.version;
		}
		pathBuilder << "_small.pti";
		return pathBuilder.Build();
	}

	void Preview::SaveInfoStart()
	{
		getSaveInfo = std::make_shared<backend::GetSaveInfo>(idv);
		getSaveInfo->Start(getSaveInfo);
	}

	void Preview::SaveInfoFinish()
	{
		if (getSaveInfo->status)
		{
			auto &info = getSaveInfo->info;

			if (saveInfoWantCommon)
			{
				auto user = backend::Backend::Ref().User();
				auto saveMine = user.detailLevel && info.username == user.name;

				saveAuthor = info.username;
				UpdateAuthorComments();

				auto *avatar = EmplaceChild<profile::AvatarButton>().get();
				avatar->HoverCursor(gui::Cursor::cursorHand);
				avatar->Position({ 7, thumbnailSize.y + 8 });
				avatar->Size({ avatarSize, avatarSize });
				avatar->Username(info.username);

				auto *views = EmplaceChild<gui::Label>().get();
				views->Size({ rightColumnWidth, FONT_H });
				views->Position({ thumbnailSize.x - rightColumnWidth - 2, thumbnailSize.y + 12 + FONT_H });
				views->Align(gui::Alignment::horizRight | gui::Alignment::vertCenter);
				views->TextRect(views->TextRect().Offset({ 2, -3 }).Grow({ 0, 6 }));
				views->Text(String::Build("DEFAULT_LS_PREVIEW_VIEWS"_Ls(), ": ", info.views));
				views->TextColor(gui::Appearance::commonLightGray);

				auto *idv = EmplaceChild<IDButton>(info.idv.id).get();
				idv->HoverCursor(gui::Cursor::cursorHand);
				idv->Size({ rightColumnWidth, FONT_H });
				idv->Position({ thumbnailSize.x - rightColumnWidth - 2, thumbnailSize.y + 12 + 2 * FONT_H });
				idv->Align(gui::Alignment::horizRight | gui::Alignment::vertCenter);

				auto *title = EmplaceChild<gui::Label>().get();
				title->Position({ 10 + avatarSize, thumbnailSize.y + 10 });
				title->Size({ thumbnailSize.x - rightColumnWidth - avatarSize - 12, FONT_H });
				title->TextRect(title->TextRect().Offset({ -1, -3 }).Grow({ 0, 6 }));
				title->Text(info.name);

				auto *author = EmplaceChild<gui::Label>().get();
				author->Position({ 10 + avatarSize, thumbnailSize.y + 12 + FONT_H });
				author->Size({ thumbnailSize.x - rightColumnWidth - avatarSize - 12, FONT_H });
				author->TextRect(author->TextRect().Offset({ -1, -3 }).Grow({ 0, 6 }));
				author->Text(info.username);
				author->TextColor(gui::Appearance::commonLightGray);

				auto *when = EmplaceChild<gui::Label>().get();
				when->Position({ 10 + avatarSize, thumbnailSize.y + 12 + 2 * FONT_H });
				when->Size({ thumbnailSize.x - rightColumnWidth - avatarSize - 12, FONT_H });
				when->TextRect(when->TextRect().Offset({ -1, -3 }).Grow({ 0, 6 }));
				String whenShort;
				String whenLong;
				if (info.createdDate == info.updatedDate)
				{
					whenShort = "DEFAULT_LS_PREVIEW_INITIAL_SHORT"_Ls(gui::Time::FormatRelative(info.createdDate, gui::Time::Now()));
					whenLong = "DEFAULT_LS_PREVIEW_INITIAL_LONG"_Ls(gui::Time::FormatAbsolute(info.createdDate));
				}
				else
				{
					whenShort = "DEFAULT_LS_PREVIEW_UPDATED_SHORT"_Ls(gui::Time::FormatRelative(info.updatedDate, gui::Time::Now()));
					whenLong = "DEFAULT_LS_PREVIEW_UPDATED_LONG"_Ls(gui::Time::FormatAbsolute(info.updatedDate), gui::Time::FormatAbsolute(info.createdDate));
				}
				when->Text(whenShort);
				when->TextColor(gui::Appearance::commonLightGray);
				when->ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ whenLong, { 0, 0 }, true }));

				auto *bars = EmplaceChild<browser::VoteBars>(info.scoreUp, info.scoreDown).get();
				bars->Position(barsPos);
				auto voteInfo = "DEFAULT_LS_PREVIEW_VOTEBAR_TIP"_Ls(info.scoreUp, info.scoreDown, info.scoreUp - info.scoreDown);
				if (info.scoreMine == 1)
				{
					if (saveMine)
					{
						voteInfo += "\n" + "DEFAULT_LS_GAME_HORIZ_LIKE_MINE"_Ls();
					}
					else
					{
						voteInfo += "\n" + "DEFAULT_LS_GAME_HORIZ_LIKE_DONE"_Ls();
					}
				}
				if (info.scoreMine == -1)
				{
					voteInfo += "\n" + "DEFAULT_LS_GAME_HORIZ_DISLIKE_DONE"_Ls();
				}
				bars->ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ voteInfo, { 0, 0 }, true }));

				auto *desc = EmplaceChild<gui::Label>().get();
				desc->Position({ 4, thumbnailSize.y + avatarSize + 13 });
				desc->Size({ thumbnailSize.x - 6, FONT_H * 6 });
				desc->TextRect(desc->TextRect().Offset({ 0, -3 }).Grow({ 0, 6 }));
				desc->Multiline(true);
				desc->Text(info.description);
				desc->TextColor(gui::Appearance::commonLightGray);

				if (user.detailLevel && (user.elevation != backend::UserInfo::elevationNone || info.username == user.name))
				{
					canManage = true;
				}
				if (info.published)
				{
					canUnpublish = true;
				}

				{
					auto pageMax = info.comments / commentsOnPage + (info.comments % commentsOnPage ? 1 : 0) - 1;
					if (pageMax < 0)
					{
						pageMax = 0;
					}
					commentPagination->PageMax(pageMax);
				}

				saveInfo = info;
				UpdateOpen();

				saveInfoWantCommon = false;
			}

			if (saveInfoWantFavorite)
			{
				favor->Stuck(info.favorite);
				favor->Enabled(true);

				saveInfoWantFavorite = false;
			}
		}
		else
		{
			// * TODO-REDO_UI: Do something with this.
		}
		getSaveInfo.reset();
	}

	void Preview::UpdateOpen()
	{
		if (!open->Enabled() && saveInfo.detailLevel && saveData)
		{
			open->Enabled(true);
			open->Click([this]() {
				game::Game::Ref().LoadOnlineSave(saveInfo, saveData);
				Quit();
				if (success)
				{
					success();
				}
			});
		}
	}

	void Preview::UpdateTasks()
	{
		if (getSaveData && getSaveData->complete)
		{
			if (getSaveData->status)
			{
				try
				{
					saveData = std::make_shared<GameSave>(getSaveData->data);
					saveData->Expand();
					gui::Point sizeB;
					saveData->BlockSizeAfterExtract(0, 0, 0, sizeB.x, sizeB.y);
					saveRender = graphics::ThumbnailRendererTask::Create({
						saveData,
						false,
						true,
						RENDER_BASC | RENDER_FIRE | RENDER_SPRK | RENDER_EFFE,
						0,
						COLOUR_DEFAULT,
						0,
						{ 0, 0 },
						{ { 0, 0 }, sizeB },
					});
					saveRender->Start(saveRender);
					UpdateOpen();
				}
				catch (const ParseException &e)
				{
					// * TODO-REDO_UI: Do something with this.
				}
			}
			else
			{
				// * TODO-REDO_UI: Do something with this.
			}
			getSaveData.reset();
		}
		if (saveRender && saveRender->complete)
		{
			if (!saveRender->status)
			{
				// * TODO-REDO_UI: Make it so that this is guaranteed to not happen.
				throw std::runtime_error("rendering paste thumbnail failed");
			}
			saveRender->output.Size(thumbnailSize, gui::Image::resizeModeLinear);
			preview->Background()->ImageData(saveRender->output);
			preview->Background()->Modulation({ 255, 255, 255, 255 });
			preview->Background()->Blend(gui::Texture::blendNone);
			previewSpinner->Visible(false);
			getThumbnail.reset();
			saveRender.reset();
		}
		if (getThumbnail && getThumbnail->complete)
		{
			// * It's fine if this fails, it isn't critical.
			if (getThumbnail->status)
			{
				getThumbnail->image.Size(thumbnailSize, gui::Image::resizeModeLanczos);
				preview->Background()->ImageData(getThumbnail->image);
				preview->Background()->Modulation({ 255, 255, 255, 128 });
				preview->Background()->Blend(gui::Texture::blendNormal);
			}
			getThumbnail.reset();
		}
		if (getSaveInfo && getSaveInfo->complete)
		{
			SaveInfoFinish();
		}
		if (getComments && getComments->complete)
		{
			CommentsFinish();
		}
		if (favorSave && favorSave->complete)
		{
			if (favorSave->status)
			{
				saveInfoWantFavorite = true;
				SaveInfoStart();
			}
			else
			{
				// * TODO-REDO_UI: Do something with this.
				favor->Enabled(true);
			}
			favorSave.reset();
		}
		if (deleteSave && deleteSave->complete)
		{
			if (deleteSave->status)
			{
				Quit();
			}
			else
			{
				// * TODO-REDO_UI: Do something with this.
			}
			deleteSave.reset();
		}
	}

	void Preview::CommentsStart(int newPage, bool resetPageBox)
	{
		if (!commentPagination->ValidPage(newPage))
		{
			return;
		}
		if (getComments) // * TODO-REDO_UI: Not great, discards valid page input if the request is still in flight.
		{
			return;
		}
		commentSpinner->Visible(true);
		commentPanel->Visible(false);
		auto *cpi = commentPanel->Interior();
		for (auto cb : commentBlocks)
		{
			cpi->RemoveChild(cb.panel);
		}
		commentBlocks.clear();
		commentPrevPage = commentPagination->Page() - newPage == 1;
		commentPagination->Page(newPage, resetPageBox);
		getComments = std::make_shared<backend::GetComments>(idv, commentPagination->Page() * commentsOnPage, commentsOnPage);
		getComments->Start(getComments);
	}

	void Preview::UpdateAuthorComments()
	{
		for (auto cb : commentBlocks)
		{
			if (cb.markIfFromAuthor && cb.username == saveAuthor)
			{
				cb.name->TextColor(gui::Appearance::commonLightRed);
			}
		}
	}

	void Preview::CommentsFinish()
	{
		commentSpinner->Visible(false);
		auto *cpi = commentPanel->Interior();
		auto top = -1;
		auto width = commentsWidth - 3;
		if (getComments->status)
		{
			if (getComments->comments.empty())
			{
				// * TODO-REDO_UI: No comments.
			}
			else
			{
				// * TODO-REDO_UI: pagination->PageMax(...) once Comments.json returns a Count
				commentPanel->Visible(true);
				for (auto &comment : getComments->comments)
				{
					auto *panel = cpi->EmplaceChild<gui::Component>().get();

					auto *name = panel->EmplaceChild<gui::Label>().get();
					name->Text(comment.user.name);

					auto whenShort = gui::Time::FormatRelative(comment.timestamp, gui::Time::Now());
					auto whenLong = gui::Time::FormatAbsolute(comment.timestamp);
					auto *date = panel->EmplaceChild<gui::Label>().get();
					date->ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ whenLong, { 0, 0 }, true }));
					date->Align(gui::Alignment::horizRight);
					date->ToFrontUnderMouse(false);
					date->Text(whenShort);
					date->TextColor(gui::Appearance::commonYellow);

					auto widthToShare = width - 15 - commentAvatarSize;
					auto dateWidth = Max(Min(widthToShare - gui::SDLWindow::Ref().TextSize(name->Text()).x - 20, gui::SDLWindow::Ref().TextSize(date->Text()).x), 50) + 7;
					date->Size({ dateWidth + 2, FONT_H });
					name->Size({ widthToShare - dateWidth, FONT_H });

					auto *body = panel->EmplaceChild<gui::Label>().get();
					body->Multiline(true);
					body->InitialInset(commentAvatarSize + 5);
					body->Size({ width - 11, 0 });
					body->Text(comment.text);
					body->TextColor(gui::Appearance::commonLightGray);
					body->ToFrontUnderMouse(false);
					body->Size({ body->Size().x, body->Lines() * FONT_H + 5 });

					auto compH = body->Size().y + FONT_H + 6;
					auto bodyY = FONT_H + 4;
					auto nameY = 6;
					if (body->Lines() > 1)
					{
						compH += 4;
						bodyY += 4;
						nameY += 2;
					}
					body->Position({ 2, bodyY });
					name->Position({ commentAvatarSize + 7, nameY });
					name->TextRect(name->TextRect().Offset({ 0, -3 }).Grow({ 0, 5 }));
					date->Position({ commentAvatarSize + 7 + name->Size().x, nameY });
					date->TextRect(date->TextRect().Offset({ 0, -3 }).Grow({ -4, 5 }));
					date->ScrollTargetRect(date->ScrollTargetRect().Grow({ 4, 0 }));

					auto *avatar = panel->EmplaceChild<profile::AvatarButton>().get();
					avatar->HoverCursor(gui::Cursor::cursorHand);
					avatar->Position({ 4, 5 });
					avatar->Size({ commentAvatarSize, commentAvatarSize });
					avatar->Username(comment.user.name);

					panel->Position({ 0, top });
					panel->Size({ width, Max(compH, commentAvatarSize + 9) });

					auto *separator = panel->EmplaceChild<gui::Separator>().get();
					separator->Position({ 0, panel->Size().y });
					separator->Size({ width, 1 });

					CommentBlock cb;
					cb.panel = panel;
					cb.name = name;
					cb.username = comment.user.name;

					switch (comment.user.elevation)
					{
					case backend::UserInfo::elevationAdmin:
						name->TextColor(gui::Appearance::commonYellow);
						break;

					case backend::UserInfo::elevationStaff:
						name->TextColor(gui::Appearance::commonLightBlue);
						break;

					default:
						cb.markIfFromAuthor = true;
						break;
					}

					top += panel->Size().y;
					commentBlocks.push_back(cb);
				}
				UpdateAuthorComments();
			}
		}
		else
		{
			// * TODO-REDO_UI: Do something with this.
		}
		cpi->Size({ width, top });
		commentPanel->Offset({ 0, commentPrevPage ? -1000000 : 0 });
		getComments.reset();
	}

	void Preview::RefreshComments()
	{
		CommentsStart(commentPagination->Page(), true);
	}

	void Preview::Tick()
	{
		PopupWindow::Tick();
		UpdateTasks();
	}
}
