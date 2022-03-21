#pragma once

#include "Component.h"
#include "Alignment.h"

namespace gui
{
	class Dragger : public Component
	{
	public:
		using DragCallback = std::function<void ()>;

	private:
		DragCallback drag;
		Rect allowedRect = { { 0, 0 }, { 0, 0 } };
		Point dragMouseOrigin;
		Point dragPosOrigin;

		void UpdateCursor();
		void ClampPosition();

	public:
		virtual void Draw() const override;
		void MouseDragMove(Point current, int button) final override;
		void MouseDragBegin(Point current, int button) final override;
		void MouseDragEnd(Point current, int button) final override;

		void Drag(DragCallback newDrag)
		{
			drag = newDrag;
		}
		DragCallback Drag() const
		{
			return drag;
		}

		void Size(Point newSize) final override;
		Point Size() const
		{
			return Component::Size();
		}

		void Position(Point newPosition) final override;
		Point Position() const
		{
			return Component::Position();
		}

		void AllowedRect(Rect newAllowedRect);
		Rect AllowedRect() const
		{
			return allowedRect;
		}
	};
}
