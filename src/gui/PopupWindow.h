#pragma once
#include "Config.h"

#include "ModalWindow.h"

namespace gui
{
	class PopupWindow : public ModalWindow
	{
	public:
		PopupWindow()
		{
			WantBackdrop(true);
		}

		void Draw() const override;
		void DrawAfterChildren() const override;
	};
}
