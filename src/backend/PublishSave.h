#pragma once
#include "Config.h"

#include "BackendTask.h"

namespace backend
{
	class PublishSave : public BackendTask
	{
		common::Task::Status Process() final override;

	public:
		PublishSave(String id);
	};
}
