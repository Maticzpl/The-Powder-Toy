#include "ImageButton.h"

#include "SDLWindow.h"
#include "Texture.h"

namespace gui
{
	ImageButton::ImageButton()
	{
		DrawBody(false);
		DrawBorder(false);
		background = EmplaceChild<Texture>().get();
	}

	void ImageButton::Draw() const
	{
		Button::Draw();
		if (background->Handle())
		{
			SDLWindow::Ref().DrawTexture(*background, backgroundRect.Offset(AbsolutePosition()));
		}
	}

	void ImageButton::Size(Point newSize)
	{
		Button::Size(newSize);
		BackgroundRect({ { 0, 0 }, newSize });
	}
}
