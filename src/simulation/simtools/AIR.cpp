#include "simulation/ToolCommon.h"
#include "simulation/Air.h"

static void draw(DRAW_FUNC_ARGS);

void SimTool::Tool_AIR()
{
	Identifier = "DEFAULT_TOOL_AIR";
	Name = "AIR";
	Colour = 0xFFFFFF;
	Draw = &draw;
}

static void draw(DRAW_FUNC_ARGS)
{
	sim->air->pv[pos.y/CELL][pos.x/CELL] += strength*0.05f;

	if (sim->air->pv[pos.y/CELL][pos.x/CELL] > 256.0f)
		sim->air->pv[pos.y/CELL][pos.x/CELL] = 256.0f;
	else if (sim->air->pv[pos.y/CELL][pos.x/CELL] < -256.0f)
		sim->air->pv[pos.y/CELL][pos.x/CELL] = -256.0f;
}
