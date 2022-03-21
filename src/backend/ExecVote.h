#pragma once
#include "Config.h"

#include "BackendTask.h"
#include "SaveIDV.h"

namespace backend
{
	class ExecVote : public BackendTask
	{
		bool Process() final override;

	public:
		ExecVote(SaveIDV idv, int direction);
	};
}
