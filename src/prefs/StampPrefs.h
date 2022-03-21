#pragma once

#include "Prefs.h"
#include "common/ExplicitSingleton.h"

namespace prefs
{
	class StampPrefs : public Prefs, public common::ExplicitSingleton<StampPrefs>
	{
	public:
		StampPrefs() : Prefs(STAMPS_DIR "/stamps")
		{
		}
	};
}
