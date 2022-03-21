#pragma once
#include "Config.h"

#include "BackendTask.h"
#include "gui/Image.h"

namespace backend
{
	class GetPTI : public BackendTask
	{
		bool Process() final override;

	public:
		gui::Image image;

		GetPTI(String path);
	};
}
