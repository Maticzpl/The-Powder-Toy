#include "simulation/ToolCommon.h"
#include "gui/Icons.h"
#include "gui/SDLWindow.h"

static void draw(DRAW_FUNC_ARGS);
void SimTool_SET_Apply(DRAW_FUNC_ARGS, void (*apply)(float dest[4], float src[4], float strength));

void SimTool::Tool_CLR()
{
	Identifier = "DEFAULT_TOOL_CLR";
	Name = gui::IconString(gui::Icons::missing, { -1, -1 });
	Colour = 0x404040;
	MenuSection = SC_DECO;
	Draw = &draw;
}

static void draw(DRAW_FUNC_ARGS)
{
	SimTool_SET_Apply(DRAW_FUNC_SUBCALL_ARGS, [](float dest[4], float src[4], float strength) {
		dest[0] = 0;
		dest[1] = 0;
		dest[2] = 0;
		dest[3] = 0;
	});
}

