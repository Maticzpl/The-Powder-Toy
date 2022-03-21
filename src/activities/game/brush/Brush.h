#pragma once

#include "gui/Point.h"

#include <array>
#include <vector>

namespace activities::game::tool
{
	class Tool;
}

namespace activities::game::brush
{
	class Brush
	{
	protected:
		bool cellAligned = false;
		gui::Point requestedRadius, effectiveRadius;
		std::vector<unsigned char> body;
		std::vector<gui::Point> outline;

		std::array<std::vector<gui::Point>, 4> offsetLists;

		virtual void GenerateBody() = 0;
		void GenerateOutline();
		void GenerateOffsetLists();
		void Generate();

		virtual void UpdateEffectiveRadius()
		{
			effectiveRadius = requestedRadius;
		}

	public:
		virtual ~Brush() = default;

		void RequestedRadius(gui::Point newRequestedRadius);
		gui::Point RequestedRadius() const
		{
			return requestedRadius;
		}

		gui::Point EffectiveRadius() const
		{
			return effectiveRadius;
		}

		gui::Point EffectiveSize() const
		{
			return 2 * effectiveRadius + gui::Point{ 1, 1 };
		}

		unsigned char &Body(gui::Point p)
		{
			return body[(p.y + effectiveRadius.y) * (effectiveRadius.x * 2 + 1) + (p.x + effectiveRadius.x)];
		}
		const unsigned char &Body(gui::Point p) const
		{
			return body[(p.y + effectiveRadius.y) * (effectiveRadius.x * 2 + 1) + (p.x + effectiveRadius.x)];
		}

		const std::vector<gui::Point> &Outline() const
		{
			return outline;
		}

		void Draw(tool::Tool *tool, gui::Point from, gui::Point to) const;

		void CellAligned(bool newCellAligned)
		{
			cellAligned = newCellAligned;
		}
		bool CellAligned() const
		{
			return cellAligned;
		}
	};
}
