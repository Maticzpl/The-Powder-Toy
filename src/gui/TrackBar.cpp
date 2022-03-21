#include "TrackBar.h"

#include "Dragger.h"
#include "Appearance.h"
#include "SDLWindow.h"

#include <SDL.h>

namespace gui
{
	class DefaultHandle : public Dragger
	{
		void Draw() const final override
		{
			auto abs = AbsolutePosition();
			auto size = Size();
			auto &cc = Dragging(SDL_BUTTON_LEFT) ? Appearance::colors.active : Appearance::colors.inactive;
			auto &c = UnderMouse() ? cc.hover : cc.idle;
			auto &g = SDLWindow::Ref();
			g.DrawRect({ abs, size }, c.body);
			g.DrawRectOutline({ abs, size }, c.border);
		}
	};

	TrackBar::TrackBar()
	{
		Handle(std::make_shared<DefaultHandle>());
	}

	void TrackBar::Handle(std::shared_ptr<Dragger> newHandle)
	{
		RemoveChild(handle);
		InsertChild(newHandle);
		handle = newHandle.get();
		handle->StealsMouse(true);
		handle->Drag([this]() {
			auto pos = handle->Position();
			auto arSize = handle->AllowedRect().size;
			if (!inclusive)
			{
				arSize += gui::Point{ 1, 1 };
			}
			auto posLow = handle->AllowedRect().pos;
			auto posHigh = posLow + arSize;
			auto posScale = arSize - handle->Size();
			auto valueScale = valueHigh - valueLow;
			value = gui::FPoint{
				(pos.x - posLow.x) * valueScale.x / posScale.x,
				(pos.y - posLow.y) * valueScale.y / posScale.y,
			};
			if (posLow.x == posHigh.x) value.x = 0;
			if (posLow.y == posHigh.y) value.y = 0;
			if (change)
			{
				change();
			}
		});
	}

	void TrackBar::Draw() const
	{
		auto abs = AbsolutePosition();
		auto size = Size();
		auto &cc = Appearance::colors.inactive;
		auto &c = Enabled() ? (UnderMouse() ? cc.hover : cc.idle) : cc.disabled;
		auto &g = SDLWindow::Ref();
		g.DrawRectOutline({ abs, size }, c.border);
	}

	void TrackBar::Size(Point newSize)
	{
		Component::Size(newSize);
		HandleRect({ { 0, 0 }, Size() });
	}

	void TrackBar::Value(FPoint newValue)
	{
		value = newValue;
		UpdateHandle();
	}

	void TrackBar::ValueLow(FPoint newValueLow)
	{
		valueLow = newValueLow;
		UpdateHandle();
	}

	void TrackBar::ValueHigh(FPoint newValueHigh)
	{
		valueHigh = newValueHigh;
		UpdateHandle();
	}

	void TrackBar::UpdateHandle()
	{
		if (handle)
		{
			auto posScale = handle->AllowedRect().size - handle->Size();
			auto valueScale = valueHigh - valueLow;
			auto pos = gui::FPoint{
				(value.x - valueLow.x) * posScale.x / valueScale.x,
				(value.y - valueLow.y) * posScale.y / valueScale.y,
			};
			if (valueLow.x == valueHigh.x) pos.x = 0;
			if (valueLow.y == valueHigh.y) pos.y = 0;
			handle->Position(handle->AllowedRect().pos + gui::Point{ int(pos.x), int(pos.y) });
		}
	}

	void TrackBar::HandleSize(Point newHandleSize)
	{
		handleSize = newHandleSize;
		if (handle)
		{
			handle->Size(handleSize);
			UpdateHandle();
		}
	}

	void TrackBar::HandleRect(Rect newHandleRect)
	{
		handleRect = newHandleRect;
		if (handle)
		{
			handle->AllowedRect(handleRect);
			UpdateHandle();
		}
	}

	void TrackBar::Inclusive(bool newInclusive)
	{
		inclusive = newInclusive;
		UpdateHandle();
	}
}
