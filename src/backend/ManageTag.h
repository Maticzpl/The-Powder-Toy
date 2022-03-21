#pragma once
#include "Config.h"

#include "BackendTask.h"

#include <set>

namespace backend
{
	class ManageTag : public BackendTask
	{
		bool Process() final override;

	public:
		std::set<String> tags;

		ManageTag(String id, String tag, bool add);
	};
}
