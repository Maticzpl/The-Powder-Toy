#pragma once
#include "Config.h"

#include "BackendTask.h"

namespace backend
{
	class PublishSave : public BackendTask
	{
		bool Process() final override;

	public:
		PublishSave(String id);
	};
}
