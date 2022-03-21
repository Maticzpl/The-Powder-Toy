#pragma once
#include "Config.h"

#include "BackendTask.h"

namespace backend
{
	class GetStartupInfo : public BackendTask
	{
		bool Process() final override;

	public:
		String motd;

		GetStartupInfo(String server);
	};
}
