#pragma once

#include "ElementTool.h"

namespace activities::game::tool
{
	class LifeElementTool : public ElementTool
	{
	public:
		LifeElementTool(int kind, String newName, String newDescription, gui::Color newColor);
	};
}
