#pragma once
#include "Config.h"

#include "Point.h"
#include "common/String.h"

namespace gui
{
	struct ToolTipInfo
	{
		String text;
		Point pos;
		bool aboveSource;
	};
}
