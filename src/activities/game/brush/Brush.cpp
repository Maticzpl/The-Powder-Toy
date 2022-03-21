#include "Brush.h"

#include "activities/game/tool/Tool.h"
#include "common/Line.h"

#include <cassert>

namespace activities::game::brush
{
	constexpr auto simulationSize = gui::Point{ XRES, YRES };
	constexpr auto simulationRect = gui::Rect{ { 0, 0 }, simulationSize };

	constexpr std::array<gui::Point, 4> offsetVectors = { {
		{  1,  0 },
		{  0,  1 },
		{ -1,  0 },
		{  0, -1 },
	} };

	void Brush::RequestedRadius(gui::Point newRequestedRadius)
	{
		requestedRadius = newRequestedRadius;
		UpdateEffectiveRadius();
		auto esize = EffectiveSize();
		auto area = esize.x * esize.y;
		body.resize(area);
		Generate();
	}

	void Brush::GenerateOutline()
	{
		outline.clear();
		for (auto y = -effectiveRadius.y; y <= effectiveRadius.y; ++y)
		{
			for (auto x = -effectiveRadius.x; x <= effectiveRadius.x; ++x)
			{
				bool set = false;
				if (Body({ x, y }))
				{
					if (x > -effectiveRadius.x && x < effectiveRadius.x && y > -effectiveRadius.y && y < effectiveRadius.y)
					{
						if (!Body({ x + 1, y }) ||
						    !Body({ x - 1, y }) ||
						    !Body({ x, y + 1 }) ||
						    !Body({ x, y - 1 }))
						{
							set = true;
						}
					}
					else
					{
						set = true;
					}
				}
				if (set)
				{
					outline.push_back({ x, y });
				}
			}
		}
	}

	void Brush::GenerateOffsetLists()
	{
		auto bodySafe = [this](gui::Point p) -> unsigned char {
			if (p.x >= -effectiveRadius.x && p.x <= effectiveRadius.x && p.y >= -effectiveRadius.y && p.y <= effectiveRadius.y)
			{
				return Body(p);
			}
			return 0;
		};
		for (auto i = 0U; i < offsetLists.size(); ++i)
		{
			auto vec = offsetVectors[i];
			auto &list = offsetLists[i];
			list.clear();
			for (auto y = -effectiveRadius.y; y <= effectiveRadius.y; ++y)
			{
				for (auto x = -effectiveRadius.x; x <= effectiveRadius.x; ++x)
				{
					auto p = gui::Point{ x, y };
					if (Body(p) && !bodySafe(p + vec))
					{
						list.push_back(p);
					}
				}
			}
		}
	}
	
	void Brush::Generate()
	{
		GenerateBody();
		GenerateOutline();
		GenerateOffsetLists();
	}

	void Brush::Draw(tool::Tool *tool, gui::Point from, gui::Point to) const
	{
		bool cont = false;
		BrushLine(from.x, from.y, to.x, to.y, [this, &cont, &from, tool](int x, int y) {
			auto curr = gui::Point{ x, y };
			if (cont && tool->UseOffsetLists())
			{
				auto diff = curr - from;
				for (int i = 0U; i < int(offsetVectors.size()); ++i)
				{
					if (offsetVectors[i] == diff)
					{
						for (auto &scan : offsetLists[i])
						{
							// Line-optmized tools get a bogus origin parameter.
							auto pos = curr + scan;
							if (simulationRect.Contains(pos))
							{
								tool->Draw(pos, { -1, -1 });
							}
						}
						break;
					}
				}
			}
			else
			{
				for (auto yy = -effectiveRadius.y; yy <= effectiveRadius.y; ++yy)
				{
					for (auto xx = -effectiveRadius.x; xx <= effectiveRadius.x; ++xx)
					{
						auto scan = gui::Point{ xx, yy };
						if (Body(scan))
						{
							auto pos = curr + scan;
							if (simulationRect.Contains(pos))
							{
								tool->Draw(pos, curr);
							}
						}
					}
				}
			}
			from = curr;
			cont = true;
		});
	}
}
