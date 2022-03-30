#pragma once
#include "Config.h"

#include "BackendTask.h"
#include "SaveIDV.h"

#include <vector>

namespace backend
{
	class GetSaveData : public BackendTask
	{
		common::Task::Status Process() final override;

	public:
		std::vector<unsigned char> data;

		GetSaveData(SaveIDV idv);
	};
}
