#pragma once
#include "Config.h"

#include "SaveIDV.h"

#include <ctime>
#include <set>

namespace backend
{
	struct SaveInfo
	{
		enum DetailLevel
		{
			detailLevelNone,
			detailLevelBasic,
			detailLevelExtended,
			detailLevelMax, // * Must be the last enum constant.
		};
		DetailLevel detailLevel;
		// * The fields below are valid if detailLevelBasic <= detailLevel < detailLevelMax.
		SaveIDV idv;
		time_t createdDate;
		time_t updatedDate;
		int scoreUp;
		int scoreDown;
		String username;
		String name;
		bool published;
		// * The fields below are valid if detailLevelExtended <= detailLevel < detailLevelMax.
		int scoreMine;
		String description;
		bool favorite;
		int comments;
		int views;
		std::set<String> tags;
	};
}
