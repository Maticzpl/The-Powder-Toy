#pragma once
#include "Config.h"

#include "BackendTask.h"

namespace backend
{
	class GetStartupInfo : public BackendTask
	{
		common::Task::Status Process() final override;

	public:
		String motd;

		GetStartupInfo(String server);
	};
}
