#include "simulation/ToolCommon.h"
#include "activities/game/Game.h"
#include "graphics/Pix.h"
#include "gui/Icons.h"
#include "gui/SDLWindow.h"

static void draw(DRAW_FUNC_ARGS);
void SimTool_SET_Apply(DRAW_FUNC_ARGS, void (*apply)(float dest[4], float src[4], float strength))
{
	if (cpart)
	{
		auto decoColor = activities::game::Game::Ref().DecoColor();
		float src[4] = {
			PixR(decoColor) / 255.f,
			PixG(decoColor) / 255.f,
			PixB(decoColor) / 255.f,
			PixA(decoColor) / 255.f,
		};
		float dest[4] = {
			PixR(cpart->dcolour) / 255.f,
			PixG(cpart->dcolour) / 255.f,
			PixB(cpart->dcolour) / 255.f,
			PixA(cpart->dcolour) / 255.f,
		};
		apply(dest, src, strength);
		auto r = int(dest[0] * 255.f);
		auto g = int(dest[1] * 255.f);
		auto b = int(dest[2] * 255.f);
		auto a = int(dest[3] * 255.f);
		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		if (a > 255) a = 255;
		if (r <   0) r =   0;
		if (g <   0) g =   0;
		if (b <   0) b =   0;
		if (a <   0) a =   0;
		cpart->dcolour = PixRGBA(r, g, b, a);
	}
}

void SimTool::Tool_SET()
{
	Identifier = "DEFAULT_TOOL_SET";
	Name = gui::IconString(gui::Icons::paint, { -1, -1 });
	Colour = 0x404040;
	MenuSection = SC_DECO;
	Draw = &draw;
}

static void draw(DRAW_FUNC_ARGS)
{
	SimTool_SET_Apply(DRAW_FUNC_SUBCALL_ARGS, [](float dest[4], float src[4], float strength) {
		dest[0] = src[0];
		dest[1] = src[1];
		dest[2] = src[2];
		dest[3] = src[3];
	});
}
