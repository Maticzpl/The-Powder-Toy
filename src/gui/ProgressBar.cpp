#include "ProgressBar.h"

#include "Format.h"
#include "Alignment.h"
#include "Appearance.h"
#include "SDLWindow.h"

namespace gui
{
	ProgressBar::ProgressBar()
	{
		barColor = Appearance::commonYellow;
		Align(Alignment::horizCenter | Alignment::vertCenter);
	}

	void ProgressBar::Draw() const
	{
		auto abs = AbsolutePosition();
		auto size = Size();
		auto &cc = Appearance::colors.inactive;
		auto &c = cc.idle;
		auto &g = SDLWindow::Ref();
		g.DrawRectOutline({ abs, size }, c.border);
		auto barSize = size - Point{ 4, 4 };
		barSize.x = int(barSize.x * progress);
		auto barRect = Rect{ abs + Point{ 2, 2 }, barSize };
		if (indeterminate)
		{
			// * TODO-REDO_UI: Adjust barRect.
		}
		g.DrawText(abs + drawTextAt, textToDraw, c.text, 0);
		g.DrawRect(barRect, barColor);
		{
			SDLWindow::NarrowClipRect ncr(barRect);
			g.DrawText(abs + drawTextAt, textToDraw, c.text, SDLWindow::drawTextInvert);
		}
	}

	void ProgressBar::UpdateStatic()
	{
		if (indeterminate)
		{
			Text("");
		}
		else
		{
			StringBuilder sb;
			sb << Format::Precision(2) << (progress * 100) << "%";
			Text(sb.Build());
		}
	}

	void ProgressBar::Progress(float newProgress)
	{
		progress = newProgress;
		UpdateStatic();
	}

	void ProgressBar::Indeterminate(bool newIndeterminate)
	{
		indeterminate = newIndeterminate;
		UpdateStatic();
	}
}
