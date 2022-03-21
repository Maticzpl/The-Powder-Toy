#pragma once
#include "Config.h"

#include "Component.h"

#include <functional>
#include <memory>

namespace gui
{
	class Dragger;

	class TrackBar : public Component
	{
	public:
		using ChangeCallback = std::function<void ()>;

	private:
		ChangeCallback change;
		Point handleSize = { 0, 0 };
		Rect handleRect = { { 0, 0 }, { 0, 0 } };
		FPoint value = { 0, 0 };
		FPoint valueLow = { 0, 0 };
		FPoint valueHigh = { 0, 0 };
		bool inclusive = true;

		Dragger *handle = nullptr;
		void UpdateHandle();

	public:
		TrackBar();

		void Draw() const override;

		void Handle(std::shared_ptr<Dragger> newHandle);
		Dragger *Handle() const
		{
			return handle;
		}

		void Change(ChangeCallback newChange)
		{
			change = newChange;
		}
		ChangeCallback Change() const
		{
			return change;
		}

		virtual void Size(Point newSize) override;
		Point Size() const
		{
			return Component::Size();
		}

		void HandleSize(Point newHandleSize);
		Point HandleSize() const
		{
			return handleSize;
		}

		void HandleRect(Rect newHandleRect);
		Rect HandleRect() const
		{
			return handleRect;
		}

		void Value(FPoint newValue);
		FPoint Value() const
		{
			return value;
		}

		void ValueLow(FPoint newValueLow);
		FPoint ValueLow() const
		{
			return valueLow;
		}

		void ValueHigh(FPoint newValueHigh);
		FPoint ValueHigh() const
		{
			return valueHigh;
		}

		void Inclusive(bool newInclusive);
		bool Inclusive() const
		{
			return inclusive;
		}
	};
}
