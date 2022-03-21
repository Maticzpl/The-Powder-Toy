#pragma once
#include "Config.h"

#include "Static.h"

namespace gui
{
	class ProgressBar : public Static
	{
		bool indeterminate = false;
		float progress = 0.f;
		Color barColor;

		void UpdateStatic();

	public:
		ProgressBar();

		void Draw() const override;

		void Progress(float newProgress);
		float Progress() const
		{
			return progress;
		}

		void Indeterminate(bool newIndeterminate);
		bool Indeterminate() const
		{
			return indeterminate;
		}
	};
}
