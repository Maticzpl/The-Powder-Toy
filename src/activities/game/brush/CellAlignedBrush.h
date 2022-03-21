#pragma once

#include "Brush.h"

namespace activities::game::brush
{
	class CellAlignedBrush : public Brush
	{
		void UpdateEffectiveRadius() final override;

	public:
		CellAlignedBrush()
		{
			CellAligned(true);
			RequestedRadius({ 0, 0 });
		}

		void GenerateBody() final override;
	};
}
