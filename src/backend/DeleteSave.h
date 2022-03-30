#pragma once
#include "Config.h"

#include "BackendTask.h"

namespace backend
{
	class DeleteSave : public BackendTask
	{
		common::Task::Status Process() final override;

	public:
		DeleteSave(String id, bool unpublishOnly);
	};
}
