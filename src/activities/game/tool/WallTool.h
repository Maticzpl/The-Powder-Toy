#pragma once

#include "Tool.h"

namespace activities::game::tool
{
	class WallTool : public Tool
	{
		int wall;

	public:
		WallTool(int wall);

		void Draw(gui::Point pos, gui::Point origin) const final override;
		void Fill(gui::Point pos) const final override;

		int Wall() const
		{
			return wall;
		}

		void GenerateTexture(gui::Image &image) const final override;
	};
}
