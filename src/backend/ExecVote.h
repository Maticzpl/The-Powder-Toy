#pragma once
#include "Config.h"

#include "BackendTask.h"
#include "SaveIDV.h"

namespace backend
{
	class ExecVote : public BackendTask
	{
		common::Task::Status Process() final override;

	public:
		ExecVote(SaveIDV idv, int direction);
	};
}
