#ifndef SIMTOOL_H
#define SIMTOOL_H

#include "common/String.h"
#include "gui/Point.h"
#include "ToolsDefs.h"

#include <cstdint>

namespace activities::game
{
	class ToolButton;
}

class Simulation;
struct Particle;
class SimTool
{
public:
	ByteString Identifier;
	String Name;
	uint32_t Colour;
	int MenuSection;
	bool UseOffsetLists;

	void (*Draw)(DRAW_FUNC_ARGS);
	void (*Fill)(FILL_FUNC_ARGS);
	void (*Line)(LINE_FUNC_ARGS);
	void (*Click)(CLICK_FUNC_ARGS);
	bool (*Select)();
	void (*DrawButton)(DRAWBUTTON_FUNC_ARGS);

#define TOOL_NUMBERS_DECLARE
#include "ToolNumbers.h"
#undef TOOL_NUMBERS_DECLARE

	SimTool();
};

#endif
