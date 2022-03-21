#include "simulation/ToolCommon.h"
#include "simulation/Air.h"

#include <cmath>

static void draw(DRAW_FUNC_ARGS);

void SimTool::Tool_CYCL()
{
	Identifier = "DEFAULT_TOOL_CYCL";
	Name = "CYCL";
	Colour = 0x132f5b;
	Draw = &draw;
}

static void draw(DRAW_FUNC_ARGS)
{
	/*
		Air velocity calculation.
		(pos.x, pos.y) -- turn 90 deg -> (-pos.y, pos.x)
	*/
	// only trigger once per cell (less laggy)
	if ((pos.x%CELL) == 0 && (pos.y%CELL) == 0)
	{
		if(origin.x == pos.x && origin.y == pos.y)
			return;

		float *vx = &sim->air->vx[pos.y / CELL][pos.x / CELL];
		float *vy = &sim->air->vy[pos.y / CELL][pos.x / CELL];

		auto dvx = float(origin.x - pos.x);
		auto dvy = float(origin.y - pos.y);
		float invsqr = 1/sqrtf(dvx*dvx + dvy*dvy);

		*vx -= (strength / 16) * (-dvy)*invsqr;
		*vy -= (strength / 16) * dvx*invsqr;

		// Clamp velocities
		if (*vx > 256.0f)
			*vx = 256.0f;
		else if (*vx < -256.0f)
			*vx = -256.0f;
		if (*vy > 256.0f)
			*vy = 256.0f;
		else if (*vy < -256.0f)
			*vy = -256.0f;

	}
}
