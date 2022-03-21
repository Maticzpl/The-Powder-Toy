#include "simulation/ToolCommon.h"

static void draw(DRAW_FUNC_ARGS);

void SimTool::Tool_AMBM()
{
	Identifier = "DEFAULT_TOOL_AMBM";
	Name = "AMBM";
	Colour = 0x00DDFF;
	Draw = &draw;
}

static void draw(DRAW_FUNC_ARGS)
{
	if (!sim->aheat_enable)
	{
		return;
	}

	sim->hv[pos.y / CELL][pos.x / CELL] -= strength * 2.0f;
	if (sim->hv[pos.y / CELL][pos.x / CELL] > MAX_TEMP) sim->hv[pos.y / CELL][pos.x / CELL] = MAX_TEMP;
	if (sim->hv[pos.y / CELL][pos.x / CELL] < MIN_TEMP) sim->hv[pos.y / CELL][pos.x / CELL] = MIN_TEMP;
}
