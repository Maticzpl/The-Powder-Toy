#include "GetStartupInfo.h"

#include "Backend.h"
#include "network/Request.h"
#include "Format.h"

namespace backend
{
	GetStartupInfo::GetStartupInfo(String server)
	{
		request->uri = (SCHEME + server + "/Startup.json").ToUtf8();
		Backend::Ref().AddAuthHeaders(*request);
	}

	common::Task::Status GetStartupInfo::Process()
	{
		auto prep = PreprocessResponse(responseCheckJson);
		if (!prep)
		{
			return prep;
		}
		try
		{
			motd = ByteString(root["MessageOfTheDay"].asString()).FromUtf8();
		}
		catch (const std::exception &e) // * TODO-REDO_UI: Stupid, should be an exception specific to our json lib.
		{
			return { false, "invalid JSON", "Could not read response: " + ByteString(e.what()).FromUtf8() };
		}
		return { true };
	}
}
