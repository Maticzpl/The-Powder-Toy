#pragma once

#include "Brush.h"

namespace activities::game::brush
{
	class RectangleBrush : public Brush
	{
	public:
		RectangleBrush()
		{
			RequestedRadius({ 0, 0 });
		}

		void GenerateBody() final override;
	};
}
