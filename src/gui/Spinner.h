#pragma once
#include "Config.h"

#include "Component.h"

namespace gui
{
	class Spinner : public Component
	{
	public:
		void Draw() const final override;
	};
}
