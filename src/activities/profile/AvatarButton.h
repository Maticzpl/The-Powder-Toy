#pragma once
#include "Config.h"

#include "gui/ImageButton.h"

#include <memory>

namespace backend
{
	class GetPTI;
}

namespace activities::profile
{
	class AvatarButton : public gui::ImageButton
	{
		std::shared_ptr<backend::GetPTI> getImage;
		String username;

	public:
		void Username(String newUsername);
		const String &Username() const
		{
			return username;
		}

		void Tick() final override;
	};
}
