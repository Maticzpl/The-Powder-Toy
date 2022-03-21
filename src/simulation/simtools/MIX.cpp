#include "simulation/ToolCommon.h"

#include "common/tpt-rand.h"
#include <cmath>

static void draw(DRAW_FUNC_ARGS);

void SimTool::Tool_MIX()
{
	Identifier = "DEFAULT_TOOL_MIX";
	Name = "MIX";
	Colour = 0xFFD090;
	Draw = &draw;
}

static void draw(DRAW_FUNC_ARGS)
{
	int thisPart = sim->pmap[pos.y][pos.x];
	if(!thisPart)
		return;

	if(random_gen() % 100 != 0)
		return;

	int distance = (int)(std::pow(strength, .5f) * 10);

	if(!(sim->elements[TYP(thisPart)].Properties & (TYPE_PART | TYPE_LIQUID | TYPE_GAS)))
		return;

	int newX = pos.x + (random_gen() % distance) - (distance/2);
	int newY = pos.y + (random_gen() % distance) - (distance/2);

	if(newX < 0 || newY < 0 || newX >= XRES || newY >= YRES)
		return;

	int thatPart = sim->pmap[newY][newX];
	if(!thatPart)
		return;

	if ((sim->elements[TYP(thisPart)].Properties&STATE_FLAGS) != (sim->elements[TYP(thatPart)].Properties&STATE_FLAGS))
		return;

	sim->pmap[pos.y][pos.x] = thatPart;
	sim->parts[ID(thatPart)].x = float(pos.x);
	sim->parts[ID(thatPart)].y = float(pos.y);

	sim->pmap[newY][newX] = thisPart;
	sim->parts[ID(thisPart)].x = float(newX);
	sim->parts[ID(thisPart)].y = float(newY);
}
