#include "VoteBars.h"

#include "gui/SDLWindow.h"
#include "gui/Appearance.h"

namespace activities::browser
{
	constexpr auto borderNormal     = gui::Color{ 0xB4, 0xB4, 0xB4, 0xFF };
	constexpr auto barFillThreshold = 8;

	VoteBars::VoteBars(int scoreUp, int scoreDown)
	{
		ScaleVoteBars(barWidthUp, barWidthDown, scaleVoteBarsTo - 4, scaleVoteBarsTo - 4, scoreUp, scoreDown);
		Size({ scaleVoteBarsTo, 13 });
	}

	void VoteBars::Draw() const
	{
		auto width = scaleVoteBarsTo;
		auto abs = AbsolutePosition();
		auto &g = gui::SDLWindow::Ref();
		g.DrawRectOutline(           { abs                                            , { width       , 7 } }, borderNormal                       );
		g.DrawRectOutline(           { abs + gui::Point{                        0, 6 }, { width       , 7 } }, borderNormal                       );
		g.DrawRect(                  { abs + gui::Point{                        1, 1 }, { width - 2   , 5 } }, gui::Appearance::voteUpBackground  );
		g.DrawRect(                  { abs + gui::Point{                        1, 7 }, { width - 2   , 5 } }, gui::Appearance::voteDownBackground);
		if (barWidthUp  ) g.DrawRect({ abs + gui::Point{ width - 2 - barWidthUp  , 2 }, { barWidthUp  , 3 } }, gui::Appearance::voteUp            );
		if (barWidthDown) g.DrawRect({ abs + gui::Point{ width - 2 - barWidthDown, 8 }, { barWidthDown, 3 } }, gui::Appearance::voteDown          );
	}

	void VoteBars::ScaleVoteBars(int &sizeUp, int &sizeDown, int maxSizeUp, int maxSizeDown, int scoreUp, int scoreDown)
	{
		auto scaleMax = std::max(scoreUp, scoreDown);
		if (scaleMax)
		{
			if (scaleMax < barFillThreshold)
			{
				scaleMax *= barFillThreshold - scaleMax;
			}
			sizeUp   = scoreUp   * maxSizeUp   / scaleMax;
			sizeDown = scoreDown * maxSizeDown / scaleMax;
		}
	}
}
