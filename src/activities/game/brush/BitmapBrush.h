#pragma once

#include "Brush.h"

#include <cstdint>

namespace activities::game::brush
{
	class BitmapBrush : public Brush
	{
		std::vector<char> data;
		gui::Point dataSize;

	public:
		BitmapBrush(const std::vector<char> &data, gui::Point dataSize) :
			data(data),
			dataSize(dataSize)
		{
			RequestedRadius({ 0, 0 });
		}

		void GenerateBody() final override;
	};
}
