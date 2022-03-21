#pragma once

#include "Tool.h"

namespace activities::game::tool
{
	class ElementTool : public Tool
	{
		int elem;

	public:
		ElementTool(int newElem, String newDescription);

		void Draw(gui::Point pos, gui::Point origin) const final override;
		void Fill(gui::Point pos) const final override;
		void GenerateTexture(gui::Image &image) const final override;
	};
}
