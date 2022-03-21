#include "simulation/ToolCommon.h"

static void draw(DRAW_FUNC_ARGS);

void SimTool::Tool_COOL()
{
	Identifier = "DEFAULT_TOOL_COOL";
	Name = "COOL";
	Colour = 0x00DDFF;
	Draw = &draw;
}

static void draw(DRAW_FUNC_ARGS)
{
	if(!cpart)
		return;
	if (cpart->type == PT_PUMP || cpart->type == PT_GPMP)
		cpart->temp -= strength*.1f;
	else
		cpart->temp -= strength*2.0f;

	if (cpart->temp > MAX_TEMP)
		cpart->temp = MAX_TEMP;
	else if (cpart->temp < 0)
		cpart->temp = 0;
}
