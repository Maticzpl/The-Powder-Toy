#include "simulation/ToolCommon.h"

static void draw(DRAW_FUNC_ARGS);

void SimTool::Tool_PGRV()
{
	Identifier = "DEFAULT_TOOL_PGRV";
	Name = "PGRV";
	Colour = 0xCCCCFF;
	Draw = &draw;
}

static void draw(DRAW_FUNC_ARGS)
{
	sim->gravmap[((pos.y/CELL)*(XRES/CELL))+(pos.x/CELL)] = strength*5.0f;
}
