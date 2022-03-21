#pragma once
#include "Config.h"

#include "gui/Rect.h"

namespace activities::browser
{
	constexpr auto windowSize  = gui::Point{ WINDOWW, WINDOWH };
	constexpr auto spinnerSize = gui::Point{ 30, 30 };
	constexpr auto saveArea    = gui::Rect{ { 8, 23 }, windowSize - gui::Point{ 16, 46 } };
	constexpr auto topArea     = gui::Rect{ { 8, 24 }, { windowSize.x - 16, 90 } };
	constexpr auto saveSpacing = gui::Point{ 2, 2 };
	constexpr auto gridSize    = gui::Point{ 5, 4 };
	constexpr auto tagGridSize = gui::Point{ 10, 4 };
	constexpr auto pageSize    = gridSize.x * gridSize.y;
	constexpr auto tagPageSize = tagGridSize.x * tagGridSize.y;
	constexpr auto controlSize = gui::Point{ 90, 17 };
}
