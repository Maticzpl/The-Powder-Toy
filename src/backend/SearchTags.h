#pragma once
#include "Config.h"

#include "BackendTask.h"
#include "TagInfo.h"

#include <vector>

namespace backend
{
	class SearchTags : public BackendTask
	{
		bool Process() final override;

	public:
		int tagCount;
		std::vector<TagInfo> tags;

		SearchTags(int start, int count);
	};
}
