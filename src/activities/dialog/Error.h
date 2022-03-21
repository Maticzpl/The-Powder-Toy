#pragma once
#include "Config.h"

#include "Message.h"

namespace activities::dialog
{
	class Error : public Message
	{
	public:
		Error(String titleText, String bodyText);
	};
}
