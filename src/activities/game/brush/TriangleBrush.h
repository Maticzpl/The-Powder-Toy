#pragma once

#include "Brush.h"

namespace activities::game::brush
{
	class TriangleBrush : public Brush
	{
	public:
		TriangleBrush()
		{
			RequestedRadius({ 0, 0 });
		}

		void GenerateBody() final override;
	};
}
