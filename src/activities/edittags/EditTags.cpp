#include "EditTags.h"

#include "gui/Appearance.h"
#include "gui/Icons.h"
#include "gui/Static.h"
#include "gui/Label.h"
#include "gui/Separator.h"
#include "gui/Button.h"
#include "gui/SDLWindow.h"
#include "gui/TextBox.h"
#include "language/Language.h"
#include "backend/Backend.h"
#include "backend/ManageTag.h"

#include <cassert>
#include <SDL.h>

namespace activities::edittags
{
	constexpr auto parentSize = gui::Point{ WINDOWW, WINDOWH };
	constexpr auto windowSize = gui::Point{ 195, 258 };

	EditTags::EditTags(backend::SaveInfo newInfo) : info(newInfo)
	{
		assert(info.detailLevel == backend::SaveInfo::detailLevelExtended);
		Size(windowSize);
		Position((parentSize - windowSize) / 2);

		auto &user = backend::Backend::Ref().User();
		canAddTags = true;
		canRemoveTags = true;
		if (!user.detailLevel)
		{
			canAddTags = false;
			canRemoveTags = false;
		}
		else if (user.elevation == backend::UserInfo::elevationNone && info.username != user.name)
		{
			canRemoveTags = false;
		}

		{
			auto title = EmplaceChild<gui::Static>();
			title->Position({ 6, 5 });
			title->Size({ windowSize.x - 20, 14 });
			title->Text("DEFAULT_LS_EDITTAGS_TITLE"_Ls());
			title->TextColor(gui::Appearance::informationTitle);
			auto sep = EmplaceChild<gui::Separator>();
			sep->Position({ 1, 22 });
			sep->Size({ windowSize.x - 2, 1 });
			auto ok = EmplaceChild<gui::Button>();
			ok->Position({ 0, windowSize.y - 17 });
			ok->Size({ windowSize.x, 17 });
			ok->Text(String::Build(gui::CommonColorString(gui::commonYellow), "DEFAULT_LS_EDITTAGS_OK"_Ls()));
			ok->Click([this]() {
				Quit();
			});
			Cancel([ok]() {
				ok->TriggerClick();
			});
			Okay([ok]() {
				ok->TriggerClick();
			});
		}

		{
			auto addButtonWidth = 40;

			tagText = EmplaceChild<gui::TextBox>().get();
			tagText->Position({ 8, windowSize.y - 40 });
			tagText->Size({ windowSize.x - addButtonWidth - 20, 16 });
			tagText->PlaceHolder(String::Build("[", "DEFAULT_LS_EDITTAGS_PLACEHOLDER"_Ls(), "]"));

			tagAdd = EmplaceChild<gui::Button>().get();
			tagAdd->Position({ windowSize.x - addButtonWidth - 8, windowSize.y - 40 });
			tagAdd->Size({ addButtonWidth, 16 });
			tagAdd->Text(
				gui::ColorString(gui::Appearance::voteUp) + gui::IconString(gui::Icons::plusMinusBody, { 0, -1 }) + gui::StepBackString() +
				gui::ColorString({ 0xFF, 0xFF, 0xFF }   ) + gui::IconString(gui::Icons::plusOutline  , { 0, -1 }) + gui::ResetString() +
				" " + "DEFAULT_LS_EDITTAGS_ADD"_Ls()
			);
			tagAdd->Click([this]() {
				auto &tag = tagText->Text();
				if (!tag.size())
				{
					return;
				}
				ManageTagStart(tag, true);
				tagText->Text("");
			});

			if (!canAddTags)
			{
				tagAdd->ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ "DEFAULT_LS_EDITTAGS_NOTLOGGEDIN"_Ls(), { 0, 0 }, true }));
				tagAdd->Enabled(false);
				tagText->Enabled(false);
			}
			else
			{
				tagAdd->ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ "DEFAULT_LS_EDITTAGS_NOTE"_Ls(), { 0, 0 }, true }));
			}
		}

		Update();
	}

	bool EditTags::KeyPress(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt)
	{
		switch (sym)
		{
		case SDLK_RETURN:
			if (ChildWithFocusDeep() == tagText)
			{
				tagAdd->TriggerClick();
				return true;
			}
			break;

		default:
			break;
		}
		return PopupWindow::KeyPress(sym, scan, repeat, shift, ctrl, alt);
	}

	void EditTags::Update()
	{
		auto removeButtonWidth = 16;

		removeButtons.clear();
		for (auto tp : tagPanels)
		{
			RemoveChild(tp);
		}
		tagPanels.clear();
		auto pos = gui::Point{ 8, 31 };
		auto push = [this, &pos, removeButtonWidth](String tag) {
			auto *panel = EmplaceChild<gui::Component>().get();
			panel->Position(pos);
			panel->Size({ windowSize.x - pos.x * 2, 16 });
			pos.y += panel->Size().y + 2;

			auto *text = panel->EmplaceChild<gui::Label>().get();
			text->Position({ 0, 0 });
			text->Size(panel->Size() - gui::Point{ removeButtonWidth, 0 });
			text->Text(tag);
			text->TextRect(text->TextRect().Offset({ -1, -1 }));

			tagPanels.push_back(panel);
			return panel;
		};
		for (auto tag : info.tags)
		{
			auto *panel = push(tag);

			auto *remove = panel->EmplaceChild<gui::Button>().get();
			remove->Position({ panel->Size().x - removeButtonWidth, 0 });
			remove->Size({ removeButtonWidth, panel->Size().y });
			remove->Text(
				gui::ColorString(gui::Appearance::voteDown) + gui::IconString(gui::Icons::plusMinusBody, { 0, -1 }) + gui::StepBackString() +
				gui::ColorString({ 0xFF, 0xFF, 0xFF }     ) + gui::IconString(gui::Icons::minusOutline , { 0, -1 })
			);
			remove->Click([this, tag]() {
				ManageTagStart(tag, false);
			});
			if (!canRemoveTags)
			{
				remove->ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ "DEFAULT_LS_EDITTAGS_NOTMINE"_Ls(), { 0, 0 }, true }));
				remove->Enabled(false);
			}

			removeButtons.push_back(remove);
		}
		if (info.tags.empty())
		{
			push(gui::CommonColorString(gui::commonLightGray) + "[" + "DEFAULT_LS_GAME_HORIZ_TAGS_EMPTY"_Ls() + "]");
		}
	}

	void EditTags::ManageTagStart(String tag, bool add)
	{
		if (manageTag)
		{
			return;
		}
		manageTag = std::make_shared<backend::ManageTag>(info.idv.id, tag, add);
		manageTag->Start(manageTag);
		tagAdd->Enabled(false);
		tagText->Enabled(false);
		for (auto rb : removeButtons)
		{
			rb->Enabled(false);
		}
	}

	void EditTags::ManageTagFinish()
	{
		if (manageTag->status)
		{
			info.tags = manageTag->tags;
			Update();
			if (change)
			{
				change();
			}
		}
		else
		{
			// * TODO-REDO_UI
		}
		if (canAddTags)
		{
			tagAdd->Enabled(true);
			tagText->Enabled(true);
		}
		for (auto rb : removeButtons)
		{
			rb->Enabled(true);
		}
		manageTag.reset();
	}

	void EditTags::Tick()
	{
		PopupWindow::Tick();
		if (manageTag && manageTag->complete)
		{
			ManageTagFinish();
		}
	}
}
