#pragma once

#include "Brush.h"

namespace activities::game::brush
{
	class EllipseBrush : public Brush
	{
		bool perfect = true;

	public:
		EllipseBrush()
		{
			RequestedRadius({ 0, 0 });
		}

		void GenerateBody() final override;

		void Perfect(bool newPerfect);
		bool Perfect() const
		{
			return perfect;
		}
	};
}
