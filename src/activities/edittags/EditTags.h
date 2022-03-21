#pragma once
#include "Config.h"

#include "gui/PopupWindow.h"
#include "backend/SaveInfo.h"
#include "common/String.h"

#include <set>
#include <vector>
#include <memory>

namespace backend
{
	class ManageTag;
}

namespace gui
{
	class TextBox;
	class Button;
}

namespace activities::edittags
{
	class EditTags : public gui::PopupWindow
	{
	public:
		using ChangeCallback = std::function<void ()>;

	private:
		ChangeCallback change;
		backend::SaveInfo info;
		bool canAddTags;
		bool canRemoveTags;
		gui::TextBox *tagText = nullptr;
		gui::Button *tagAdd = nullptr;

		std::shared_ptr<backend::ManageTag> manageTag;
		void ManageTagStart(String tag, bool add);
		void ManageTagFinish();

		std::vector<gui::Component *> tagPanels;
		std::vector<gui::Button *> removeButtons;

		void Update();

	public:
		EditTags(backend::SaveInfo newInfo);

		void Tick() final override;
		bool KeyPress(int sym, int scan, bool repeat, bool shift, bool ctrl, bool alt) final override;
		
		void Change(ChangeCallback newChange)
		{
			change = newChange;
		}
		ChangeCallback Change() const
		{
			return change;
		}

		const std::set<String> &Tags() const
		{
			return info.tags;
		}
	};
}
