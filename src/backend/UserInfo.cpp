#include "UserInfo.h"

namespace backend
{
	UserInfo::Elevation UserInfo::ElevationFromString(const String &elevationStr)
	{
		if (elevationStr == "Admin")
		{
			return elevationAdmin;
		}
		if (elevationStr == "Mod")
		{
			return elevationStaff;
		}
		return elevationNone;
	}
}
