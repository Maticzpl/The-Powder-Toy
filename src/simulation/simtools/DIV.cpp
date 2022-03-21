#include "simulation/ToolCommon.h"

static void draw(DRAW_FUNC_ARGS);
void SimTool_SET_Apply(DRAW_FUNC_ARGS, void (*apply)(float dest[4], float src[4], float strength));

void SimTool::Tool_DIV()
{
	Identifier = "DEFAULT_TOOL_DIV";
	Name = "/";
	Colour = 0x404040;
	MenuSection = SC_DECO;
	Draw = &draw;
}

static void draw(DRAW_FUNC_ARGS)
{
	SimTool_SET_Apply(DRAW_FUNC_SUBCALL_ARGS, [](float dest[4], float src[4], float strength) {
		dest[0] /= 1.f + src[0] * strength * 0.1f * src[3];
		dest[1] /= 1.f + src[1] * strength * 0.1f * src[3];
		dest[2] /= 1.f + src[2] * strength * 0.1f * src[3];
	});
}

