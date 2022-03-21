#pragma once
#include "Config.h"

#include "BackendTask.h"
#include "SaveInfo.h"

namespace backend
{
	class GetSaveInfo : public BackendTask
	{
		bool Process() final override;

	public:
		SaveInfo info;

		GetSaveInfo(SaveIDV idv);
	};
}
