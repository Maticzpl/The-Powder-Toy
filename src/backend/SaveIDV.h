#pragma once
#include "Config.h"

#include "common/String.h"

#include <cstdint>

namespace backend
{
	struct SaveIDV
	{
		String id;
		int64_t version; // * Do not use this to format dates, use SaveInfo::createdDate and SaveInfo::updatedDate.
	};
}
