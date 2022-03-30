#pragma once
#include "Config.h"

#include "BackendTask.h"
#include "SaveIDV.h"

#include <vector>

namespace backend
{
	class PutSave : public BackendTask
	{
		common::Task::Status Process() final override;

	public:
		String id;

		PutSave(String name, String description, bool publish, const std::vector<char> saveData);
	};
}
