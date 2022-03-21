#pragma once
#include "Config.h"

#include "BackendTask.h"

namespace backend
{
	class FavorSave : public BackendTask
	{
		bool Process() final override;

	public:
		FavorSave(String id, bool favor);
	};
}
