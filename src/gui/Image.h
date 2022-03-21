#pragma once
#include "Config.h"

#include "Point.h"

#include <cstdint>
#include <cstddef>
#include <vector>

namespace gui
{
	class Image
	{
		Point size = { 0, 0 };
		std::vector<uint32_t> pixels;

	public:
		static Image FromPTI(const char *data, size_t size);
		static Image FromBMP(const char *data, size_t size);
		static Image FromSize(Point size);
		static Image FromColor(uint32_t color, Point size);

		enum ResizeMode
		{
			resizeModeNone,
			resizeModeLinear,
			resizeModeLanczos,
			resizeModeMax, // * Must be the last enum constant.
		};
		void Size(Point newSize, ResizeMode resizeMode = resizeModeNone);
		Point Size() const
		{
			return size;
		}

		bool Empty() const
		{
			return size.x == 0 || size.y == 0;
		}

		uint32_t &operator ()(int x, int y)
		{
			return pixels[y * size.x + x];
		}

		const uint32_t &operator ()(int x, int y) const
		{
			return pixels[y * size.x + x];
		}

		uint32_t *Pixels()
		{
			return &pixels[0];
		}

		size_t PixelsSize() const
		{
			return pixels.size();
		}
	};
}
