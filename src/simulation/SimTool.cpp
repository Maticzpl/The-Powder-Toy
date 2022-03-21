#include <cmath>

#include "common/tpt-rand.h"
#include "graphics/SimulationRenderer.h"
#include "ElementGraphics.h"
#include "Gravity.h"
#include "Simulation.h"
#include "SimulationData.h"

#include "Misc.h"
#include "ToolClasses.h"

SimTool::SimTool() :
	Identifier("DEFAULT_TOOL_INVALID"),
	Name(""),
	Colour(0xFF00FF),
	MenuSection(SC_TOOL),
	UseOffsetLists(false),

	Draw(nullptr),
	Fill(nullptr),
	Line(nullptr),
	Click(nullptr),
	Select(nullptr),
	DrawButton(nullptr)
{
}
