#pragma once
#include "Config.h"

#include "BackendTask.h"
#include "CommentInfo.h"
#include "SaveIDV.h"

#include <vector>

namespace backend
{
	class GetComments : public BackendTask
	{
		common::Task::Status Process() final override;

	public:
		std::vector<CommentInfo> comments;

		GetComments(SaveIDV idv, int start, int count);
	};
}
