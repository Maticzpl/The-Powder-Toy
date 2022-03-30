#pragma once
#include "Config.h"

#include "BackendTask.h"
#include "SaveInfo.h"

namespace backend
{
	class GetSaveInfo : public BackendTask
	{
		common::Task::Status Process() final override;

	public:
		SaveInfo info;

		GetSaveInfo(SaveIDV idv);
	};
}
