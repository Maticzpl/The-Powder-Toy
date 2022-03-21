#include "simulation/ToolCommon.h"

static void draw(DRAW_FUNC_ARGS);

void SimTool::Tool_NGRV()
{
	Identifier = "DEFAULT_TOOL_NGRV";
	Name = "NGRV";
	Colour = 0xAACCFF;
	Draw = &draw;
}

static void draw(DRAW_FUNC_ARGS)
{
	sim->gravmap[((pos.y/CELL)*(XRES/CELL))+(pos.x/CELL)] = strength*-5.0f;
}
