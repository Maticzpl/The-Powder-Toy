#include "CellAlignedBrush.h"

#include "Config.h"

namespace activities::game::brush
{
	static int adjustSize(int size)
	{
		return ((size / CELL) * 2 + 1) * CELL;
	}

	void CellAlignedBrush::UpdateEffectiveRadius()
	{
		effectiveRadius = { adjustSize(requestedRadius.x) / 2, adjustSize(requestedRadius.y) / 2 };
	}

	void CellAlignedBrush::GenerateBody()
	{
		auto includeLastCol = bool(adjustSize(RequestedRadius().x) & 1);
		auto includeLastRow = bool(adjustSize(RequestedRadius().y) & 1);
		for (auto y = -effectiveRadius.y; y <= effectiveRadius.y; ++y)
		{
			for (auto x = -effectiveRadius.x; x <= effectiveRadius.x; ++x)
			{
				Body({ x, y }) = (x < effectiveRadius.x || includeLastCol) && (y < effectiveRadius.y || includeLastRow) ? 1 : 0;
			}
		}
	}
}
