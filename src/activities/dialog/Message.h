#pragma once
#include "Config.h"

#include "Dialog.h"

namespace activities::dialog
{
	class Message : public Dialog
	{
	public:
		using FinishCallback = std::function<void ()>;

	private:
		FinishCallback finish;

	public:
		Message(String titleText, String bodyText);

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
