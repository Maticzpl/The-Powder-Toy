#include "Static.h"

#include "SDLWindow.h"
#include "Appearance.h"

namespace gui
{
	Static::Static() : textColor(Appearance::colors.inactive.idle.text)
	{
	}

	void Static::Draw() const
	{
		SDLWindow::Ref().DrawText(AbsolutePosition() + drawTextAt, textToDraw, textColor, Enabled() ? 0 : SDLWindow::drawTextDarken);
	}

	void Static::UpdateText()
	{
		Point ts;
		SDLWindow::Ref().TruncateText(textToDraw, ts, text, textRect.size.x, overflowText);

		switch (align & Alignment::horizBits)
		{
		case Alignment::horizLeft:
			drawTextAt.x = 2;
			break;
		
		case Alignment::horizCenter:
			drawTextAt.x = (textRect.size.x - ts.x) - (textRect.size.x - ts.x) / 2;
			break;
		
		case Alignment::horizRight:
			drawTextAt.x = textRect.size.x - ts.x - 2;
			break;
		}

		switch (align & Alignment::vertBits)
		{
		case Alignment::vertTop:
			drawTextAt.y = 2;
			break;
		
		case Alignment::vertCenter:
			drawTextAt.y = (textRect.size.y - ts.y) - (textRect.size.y - ts.y) / 2;
			break;
		
		case Alignment::vertBottom:
			drawTextAt.y = textRect.size.y - ts.y - 2;
			break;
		}

		drawTextAt += textRect.pos;
	}

	void Static::Align(uint32_t newAlign)
	{
		align = newAlign;
		UpdateText();
	}

	void Static::Text(const String &newText)
	{
		text = newText;
		UpdateText();
	}

	void Static::OverflowText(const String &newOverflowText)
	{
		overflowText = newOverflowText;
		UpdateText();
	}

	void Static::TextRect(Rect newTextRect)
	{
		textRect = newTextRect;
		UpdateText();
	}

	void Static::Size(Point newSize)
	{
		Component::Size(newSize);
		TextRect({ { 0, 0 }, newSize });
	}
}
