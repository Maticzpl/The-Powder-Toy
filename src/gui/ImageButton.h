#pragma once
#include "Config.h"

#include "Button.h"

namespace gui
{
	class Texture;

	class ImageButton : public Button
	{
		Texture *background = nullptr;
		Rect backgroundRect = { { 0, 0 }, { 0, 0 } };

	public:
		ImageButton();
		virtual void Draw() const override;

		Texture *Background() const
		{
			return background;
		}

		void Size(Point newSize) override;
		Point Size() const
		{
			return Button::Size();
		}

		void BackgroundRect(Rect newBackgroundRect)
		{
			backgroundRect = newBackgroundRect;
		}
		Rect BackgroundRect() const
		{
			return backgroundRect;
		}
	};
}
