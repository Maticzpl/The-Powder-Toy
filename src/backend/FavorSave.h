#pragma once
#include "Config.h"

#include "BackendTask.h"

namespace backend
{
	class FavorSave : public BackendTask
	{
		common::Task::Status Process() final override;

	public:
		FavorSave(String id, bool favor);
	};
}
