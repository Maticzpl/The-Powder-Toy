#include "RectangleBrush.h"

namespace activities::game::brush
{
	void RectangleBrush::GenerateBody()
	{
		for (auto y = -effectiveRadius.y; y <= effectiveRadius.y; ++y)
		{
			for (auto x = -effectiveRadius.x; x <= effectiveRadius.x; ++x)
			{
				Body({ x, y }) = 1;
			}
		}
	}
}
