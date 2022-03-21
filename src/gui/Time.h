#pragma once
#include "Config.h"

#include "common/String.h"

#include <ctime>

namespace gui
{
namespace Time
{
	time_t Now();
	struct tm LocalTime(time_t time);
	String FormatAbsolute(time_t time);
	String FormatRelative(time_t time, time_t ref);
}
}
