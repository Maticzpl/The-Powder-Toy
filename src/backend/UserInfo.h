#pragma once

#include "Config.h"
#include "common/String.h"

namespace backend
{
	struct UserInfo
	{
		enum DetailLevel
		{
			detailLevelNone,
			detailLevelBasic,
			detailLevelSession,
			detailLevelMax, // * Must be the last enum constant.
		};
		DetailLevel detailLevel;
		// * The fields below are valid if detailLevelBasic <= detailLevel < detailLevelMax.
		uint64_t id;
		String name;
		enum Elevation
		{
			elevationNone,
			elevationStaff,
			elevationAdmin,
			elevationMax, // * Must be the last enum constant.
		};
		Elevation elevation;
		// * The fields below are valid if detailLevelSession <= detailLevel < detailLevelMax.
		String session;
		String antiCsrf;

		static Elevation ElevationFromString(const String &elevationStr);
	};
}
