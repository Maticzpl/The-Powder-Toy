#pragma once

#include "Tool.h"

namespace activities::game::tool
{
	class SimulationTool : public Tool
	{
		int simple;

	public:
		SimulationTool(int newSimple, String newDescription);

		void Draw(gui::Point pos, gui::Point origin) const final override;
		void Fill(gui::Point pos) const final override;
		void Line(gui::Point from, gui::Point to) const final override;
		void Click(gui::Point pos) const final override;
		bool Select() final override;
		void DrawButton(const ToolButton *toolButton) const final override;
	};
}
