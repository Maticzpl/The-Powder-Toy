#pragma once
#include "Config.h"

#include "BackendTask.h"
#include "SaveInfo.h"
#include "SearchDomain.h"
#include "SearchSort.h"

#include <vector>

namespace backend
{
	class SearchSaves : public BackendTask
	{
		common::Task::Status Process() final override;

	public:
		int saveCount;
		std::vector<SaveInfo> saves;

		SearchSaves(String query, int start, int count, SearchDomain searchDomain, SearchSort searchSort);
	};
}
