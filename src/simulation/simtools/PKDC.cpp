#include "simulation/ToolCommon.h"
#include "gui/SDLWindow.h"
#include "gui/Appearance.h"
#include "activities/colorpicker/ColorPicker.h"
#include "activities/game/Game.h"
#include "activities/game/ToolButton.h"
#include "graphics/Pix.h"

static bool select();
static void drawButton(DRAWBUTTON_FUNC_ARGS);

void SimTool::Tool_PKDC()
{
	Identifier = "DEFAULT_TOOL_PKDC";
	Name = "PKDC";
	Colour = 0x404040;
	MenuSection = SC_DECO;
	Select = &select;
	DrawButton = &drawButton;
}

static bool select()
{
	gui::SDLWindow::Ref().EmplaceBack<activities::colorpicker::ColorPicker>();
	return true;
}

static void drawButton(DRAWBUTTON_FUNC_ARGS)
{
	auto dcolour = activities::game::Game::Ref().DecoColor();
	auto abs = toolButton->AbsolutePosition();
	auto size = toolButton->Size();
	auto &g = gui::SDLWindow::Ref();
	auto pos = abs + (size - gui::Point{ 24, 12 }) / 2;
	auto color = gui::Color{ PixR(dcolour), PixG(dcolour), PixB(dcolour), PixA(dcolour) };
	g.DrawRectOutline({ abs + gui::Point{ 2, 2 }, activities::game::ToolButton::bodySize }, gui::Appearance::darkColor(color) ? gui::Color{ 0xC0, 0xC0, 0xC0, 0xFF } : gui::Color{ 0x40, 0x40, 0x40, 0xFF });
	g.DrawRect({ pos, { 12, 12 } }, { PixR(dcolour), PixG(dcolour), PixB(dcolour), 0xFF });
	for (auto y = 0; y < 4; ++y)
	{
		for (auto x = 0; x < 4; ++x)
		{
			g.DrawRect({ pos + gui::Point{ 12 + x * 3, y * 3 }, { 3, 3 } }, ((x + y) & 1) ? gui::Color{ 0xFF, 0xFF, 0xFF, 0xFF } : gui::Color{ 0xC0, 0xC0, 0xC0, 0xFF });
			g.DrawRect({ pos + gui::Point{ 12 + x * 3, y * 3 }, { 3, 3 } }, color);
		}
	}
}
