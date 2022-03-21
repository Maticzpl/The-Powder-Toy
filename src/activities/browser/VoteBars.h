#pragma once
#include "Config.h"

#include "gui/Component.h"

namespace activities::browser
{
	constexpr auto scaleVoteBarsTo = 60;

	class VoteBars : public gui::Component
	{
		int barWidthUp;
		int barWidthDown;
		bool barsValid = false;

	public:
		VoteBars(int scoreUp, int scoreDown);

		virtual void Draw() const final override;

		static void ScaleVoteBars(int &sizeUp, int &sizeDown, int maxSizeUp, int maxSizeDown, int scoreUp, int scoreDown);
	};
}

