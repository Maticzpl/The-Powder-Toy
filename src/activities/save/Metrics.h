#pragma once
#include "Config.h"

#include "gui/Rect.h"

namespace activities::save
{
	constexpr auto parentSize     = gui::Point{ WINDOWW, WINDOWH };
	constexpr auto thumbnailSize  = gui::Point{ XRES, YRES } / 6;
	constexpr auto maxStatusLines = 4;
	constexpr auto spinnerSize    = gui::Point{ 30, 30 };
}
