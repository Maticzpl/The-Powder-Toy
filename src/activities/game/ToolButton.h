#pragma once

#include "gui/Button.h"

namespace gui
{
	class Texture;
}

namespace activities::game::tool
{
	class Tool;
}

namespace activities::game
{
	class ToolButton : public gui::Button
	{
		String identifier;
		tool::Tool *tool;
		bool allowReplace;
		bool favorite;
		gui::Color bg;
		gui::Color fg;

		gui::Texture *texture = nullptr;
		void UpdateTexture(bool rendererUp);
		void UpdateFavorite();

	public:
		ToolButton(String identifier, tool::Tool *tool);

		void HandleClick(int button) final override;
		void UpdateText() final override;
		void Draw() const final override;

		void ToolTipPosition(gui::Point toolTipPosition);
		virtual void Select(int toolIndex);

		static constexpr auto bodySize = gui::Point{ 26, 14 };
	};
}
