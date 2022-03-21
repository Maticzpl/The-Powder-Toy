#include "ToolButton.h"

#include "Game.h"
#include "tool/Tool.h"
#include "gui/SDLWindow.h"
#include "gui/Texture.h"
#include "gui/Icons.h"
#include "gui/Appearance.h"

#include <SDL.h>

namespace activities::game
{
	ToolButton::ToolButton(String identifier, tool::Tool *tool) :
		identifier(identifier),
		tool(tool),
		allowReplace(tool->AllowReplace())
	{
		Size(bodySize + gui::Point{ 4, 4 });
		Text(tool->Name());
		bg = tool->Color();
		fg = gui::Appearance::darkColor(bg) ? gui::Color{ 255, 255, 255, 255 } : gui::Color{ 0, 0, 0, 255 };
		texture = EmplaceChild<gui::Texture>().get();
		if (tool->CanGenerateTexture())
		{
			auto image = gui::Image::FromColor(UINT32_C(0xFFFF00FF), bodySize);
			tool->GenerateTexture(image);
			texture->ImageData(image);
		}
		UpdateFavorite();
	}

	void ToolButton::Select(int toolIndex)
	{
		Game::Ref().CurrentTool(toolIndex, tool);
	}

	void ToolButton::HandleClick(int button)
	{
		auto &g = gui::SDLWindow::Ref();
		switch (uint64_t(button) | (uint64_t(g.Mod()) << 48))
		{
		case uint64_t(SDL_BUTTON_LEFT):
			Select(0);
			break;

		case uint64_t(SDL_BUTTON_RIGHT):
			Select(1);
			break;

		case uint64_t(SDL_BUTTON_MIDDLE):
			Select(2);
			break;

		case uint64_t(SDL_BUTTON_LEFT) | (uint64_t(gui::SDLWindow::mod1 | gui::SDLWindow::mod2) << 48):
			Select(3);
			break;

		case uint64_t(SDL_BUTTON_LEFT) | (uint64_t(gui::SDLWindow::mod0 | gui::SDLWindow::mod1) << 48):
			game::Game::Ref().Favorite(identifier, !favorite);
			UpdateFavorite();
			break;
		}
	}

	void ToolButton::UpdateText() // * ToolButtons have strict text alignment requirements.
	{
		auto ts = gui::SDLWindow::Ref().TextSize(text) - gui::Point{ 1, 0 };
		drawTextAt.x = (textRect.size.x - ts.x) - (textRect.size.x - ts.x) / 2;
		drawTextAt.y = (textRect.size.y - ts.y) - (textRect.size.y - ts.y) / 2;
		drawTextAt += textRect.pos;
	}

	void ToolButton::UpdateFavorite()
	{
		favorite = game::Game::Ref().Favorite(identifier);
	}

	void ToolButton::ToolTipPosition(gui::Point toolTipPosition)
	{
		ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ tool->Description(), toolTipPosition }));
	}

	void ToolButton::Draw() const
	{
		auto abs = AbsolutePosition();
		auto size = Size();
		auto &g = gui::SDLWindow::Ref();
		if (tool->CanDrawButton())
		{
			tool->DrawButton(this);
		}
		else if (texture->Handle())
		{
			g.DrawTexture(*texture, { abs + gui::Point{ 2, 2 }, bodySize });
		}
		else
		{
			g.DrawRect({ abs + gui::Point{ 2, 2 }, bodySize }, bg);
			g.DrawText(abs + drawTextAt, text, fg);
		}
		if (Game::Ref().CurrentTool(0) == tool)
		{
			g.DrawRectOutline({ abs, size }, { 0xFF, 0x50, 0x30, 0xFF });
		}
		else if (Game::Ref().CurrentTool(1) == tool)
		{
			g.DrawRectOutline({ abs, size }, { 0x30, 0xA0, 0xFF, 0xFF });
		}
		else if (Game::Ref().CurrentTool(2) == tool)
		{
			g.DrawRectOutline({ abs, size }, { 0x30, 0xFF, 0x50, 0xFF });
		}
		else if (Game::Ref().CurrentTool(3) == tool)
		{
			g.DrawRectOutline({ abs, size }, { 0xA0, 0x50, 0xFF, 0xFF });
		}
		else if (UnderMouse())
		{
			g.DrawRectOutline({ abs, size }, { 0xC0, 0xC0, 0xC0, 0xFF });
		}
		if (favorite)
		{
			g.DrawText(abs + gui::Point{ 0, -2 }, String(gui::Icons::smallHeart), { 0xFF, 0xFF, 0x00, 0xFF });
		}
	}
}
