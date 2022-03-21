#include "Dragger.h"

#include "SDLWindow.h"
#include "Appearance.h"

#include <SDL.h>

namespace gui
{
	void Dragger::MouseDragMove(Point current, int button)
	{
		Component::Position(dragPosOrigin - dragMouseOrigin + current);
		ClampPosition();
		if (drag)
		{
			drag();
		}
	}

	void Dragger::MouseDragBegin(Point current, int button)
	{
		if (button == SDL_BUTTON_LEFT)
		{
			CaptureMouse(true);
			auto apos = AbsolutePosition();
			auto size = Size();
			for (auto dim = 0; dim < 2; ++dim)
			{
				if (apos[dim] > current[dim] || apos[dim] + size[dim] <= current[dim])
				{
					apos[dim] = current[dim] - size[dim] / 2;
				}
			}
			dragPosOrigin = apos - Parent()->AbsolutePosition();
			dragMouseOrigin = current;
			MouseDragMove(current, button);
		}
	}

	void Dragger::Draw() const
	{
		auto abs = AbsolutePosition();
		auto size = Size();
		auto &cc = Dragging(SDL_BUTTON_LEFT) ? Appearance::colors.active : Appearance::colors.inactive;
		auto &c = UnderMouse() ? cc.hover : cc.idle;
		auto &g = SDLWindow::Ref();
		g.DrawRect({ abs + Point{ 1, 1 }, size - Point{ 2, 2 } }, c.border);
	}

	void Dragger::MouseDragEnd(Point current, int button)
	{
		CaptureMouse(false);
	}

	void Dragger::Size(Point newSize)
	{
		Component::Size(newSize);
		UpdateCursor();
		ClampPosition();
	}

	void Dragger::UpdateCursor()
	{
		auto canSizewe = Size().x < allowedRect.size.x;
		auto canSizens = Size().y < allowedRect.size.y;
		HoverCursor(
			canSizewe ?
			(canSizens ? Cursor::cursorSizeall : Cursor::cursorSizewe) :
			(canSizens ? Cursor::cursorSizens : Cursor::cursorArrow)
		);
	}

	void Dragger::ClampPosition()
	{
		Component::Position(allowedRect.Grow(gui::Point{ 1, 1 } - Size()).Clamp(Position()));
	}

	void Dragger::AllowedRect(Rect newAllowedRect)
	{
		if (allowedRect != newAllowedRect)
		{
			allowedRect = newAllowedRect;
			UpdateCursor();
			ClampPosition();
		}
	}

	void Dragger::Position(Point newPosition)
	{
		Component::Position(newPosition);
		ClampPosition();
	}
}
