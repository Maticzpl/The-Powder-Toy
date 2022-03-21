#pragma once

#include "Prefs.h"
#include "common/ExplicitSingleton.h"

namespace prefs
{
	class GlobalPrefs : public Prefs, public common::ExplicitSingleton<GlobalPrefs>
	{
	public:
		GlobalPrefs() : Prefs("powder")
		{
		}
	};
}
