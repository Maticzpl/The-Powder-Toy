#pragma once
#include "Config.h"

#include "Dialog.h"

namespace activities::dialog
{
	class Confirm : public Dialog
	{
	public:
		using FinishCallback = std::function<void (bool confirmed)>;

	private:
		FinishCallback finish;

	public:
		Confirm(String titleText, String bodyText);

		void Finish(FinishCallback newFinish)
		{
			finish = newFinish;
		}
		FinishCallback Finish() const
		{
			return finish;
		}
	};
}
