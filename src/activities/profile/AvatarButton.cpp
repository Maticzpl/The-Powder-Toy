#include "AvatarButton.h"

#include "backend/GetPTI.h"
#include "gui/Texture.h"
#include "gui/SDLWindow.h"
#include "gui/Icons.h"

namespace activities::profile
{
	void AvatarButton::Username(String newUsername)
	{
		username = newUsername;
		Background()->ImageData(gui::Image());
		Text("");
		ToolTip(nullptr);
		if (username.size())
		{
			getImage = std::make_shared<backend::GetPTI>(String::Build("/avatars/", username, ".pti"));
			getImage->Start(getImage);
		}
	}

	void AvatarButton::Tick()
	{
		if (getImage && getImage->complete)
		{
			if (getImage->status)
			{
				getImage->image.Size(Size(), gui::Image::resizeModeLanczos);
				Background()->ImageData(getImage->image);
			}
			else
			{
				Text(String::Build(gui::CommonColorString(gui::commonYellow), getImage->shortError));
				ToolTip(std::make_shared<gui::ToolTipInfo>(gui::ToolTipInfo{ getImage->error, { 0, 0 }, true }));
			}
			getImage.reset();
		}
	}
}
