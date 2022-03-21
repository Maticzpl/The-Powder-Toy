#include "LifeElementTool.h"

#include "simulation/ElementClasses.h"

namespace activities::game::tool
{
	LifeElementTool::LifeElementTool(int kind, String newName, String newDescription, gui::Color newColor) :
		ElementTool(PMAP(kind, PT_LIFE), newDescription)
	{
		MenuVisible(true);
		Name(newName);
		Color(newColor);
	}
}
