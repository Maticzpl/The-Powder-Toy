#include "simulation/ToolCommon.h"

static void draw(DRAW_FUNC_ARGS);

void SimTool::Tool_AMBP()
{
	Identifier = "DEFAULT_TOOL_AMBP";
	Name = "AMBP";
	Colour = 0xFFDD00;
	Draw = &draw;
}

static void draw(DRAW_FUNC_ARGS)
{
	if (!sim->aheat_enable)
	{
		return;
	}

	sim->hv[pos.y / CELL][pos.x / CELL] += strength * 2.0f;
	if (sim->hv[pos.y / CELL][pos.x / CELL] > MAX_TEMP) sim->hv[pos.y / CELL][pos.x / CELL] = MAX_TEMP;
	if (sim->hv[pos.y / CELL][pos.x / CELL] < MIN_TEMP) sim->hv[pos.y / CELL][pos.x / CELL] = MIN_TEMP;
}
