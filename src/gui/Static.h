#pragma once

#include "Color.h"
#include "Component.h"
#include "Alignment.h"
#include "common/String.h"

namespace gui
{
	class Static : public Component
	{
	protected:
		String text;
		String textToDraw;
		String overflowText = "...";
		uint32_t align = Alignment::horizLeft | Alignment::vertCenter;
		Point drawTextAt = { 0, 0 };
		Rect textRect = { { 0, 0 }, { 0, 0 } };
		Color textColor;
		virtual void UpdateText();

	public:
		Static();
		
		virtual void Draw() const override;

		virtual void Text(const String &newText);
		virtual const String &Text() const
		{
			return text;
		}

		void OverflowText(const String &newOverflowText);
		const String &OverflowText() const
		{
			return overflowText;
		}

		void Align(uint32_t newAlign);
		const uint32_t Align() const
		{
			return align;
		}

		virtual void Size(Point newSize) override;
		Point Size() const
		{
			return Component::Size();
		}

		void TextRect(Rect newTextRect);
		Rect TextRect() const
		{
			return textRect;
		}

		void TextColor(Color newTextColor)
		{
			textColor = newTextColor;
		}
		Color TextColor() const
		{
			return textColor;
		}
	};
}
