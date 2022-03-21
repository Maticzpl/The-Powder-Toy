#include "simulation/ToolCommon.h"
#include "activities/game/Game.h"
#include "activities/game/brush/Brush.h"

constexpr auto simulationSize = gui::Point{ XRES, YRES };
constexpr auto simulationRect = gui::Rect{ { 0, 0 }, simulationSize };

static void line(LINE_FUNC_ARGS);

void SimTool::Tool_WIND()
{
	Identifier = "DEFAULT_TOOL_WIND";
	Name = "WIND";
	Colour = 0x404040;
	Line = &line;
}

static void line(LINE_FUNC_ARGS)
{
	strength *= 0.01f;
	auto &game = activities::game::Game::Ref();
	auto *br = game.Brushes()[game.CurrentBrush()].get();
	auto dx = (to.x - from.x) * strength;
	auto dy = (to.y - from.y) * strength;
	auto effectiveRadius = br->EffectiveRadius();
	for (auto yy = -effectiveRadius.y; yy <= effectiveRadius.y; ++yy)
	{
		for (auto xx = -effectiveRadius.x; xx <= effectiveRadius.x; ++xx)
		{
			auto scan = gui::Point{ xx, yy };
			if (br->Body(scan))
			{
				auto pos = from + scan;
				if (simulationRect.Contains(pos))
				{
					sim->vx[pos.y / CELL][pos.x / CELL] += dx;
					sim->vy[pos.y / CELL][pos.x / CELL] += dy;
				}
			}
		}
	}
}
