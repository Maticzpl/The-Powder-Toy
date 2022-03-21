#include "Startup.h"

#include "GetStartupInfo.h"
#include "Backend.h"

#include <iostream>

namespace backend
{
	Startup::Startup()
	{
		getStartupInfos[0] = std::make_shared<backend::GetStartupInfo>(SERVER);
		toComplete += 1;
#ifdef UPDATESERVER
		getStartupInfos[1] = std::make_shared<backend::GetStartupInfo>(UPDATESERVER);
		toComplete += 1;
#endif
		for (auto &gsi : getStartupInfos)
		{
			gsi->Start(gsi);
		}
	}

	bool GetMotdUri(String &text, String &uri, const String &raw)
	{
		auto openAt = raw.find(String("{a:"));
		if (openAt != raw.npos)
		{
			auto textAt = raw.find(String("|"), openAt + 1);
			if (textAt != raw.npos)
			{
				auto closeAt = raw.find(String("}"), textAt + 1);
				if (closeAt)
				{
					uri = raw.substr(openAt + 3, textAt - openAt - 3);
					text = raw.substr(0, openAt) + raw.substr(textAt + 1, closeAt - textAt - 1) + raw.substr(closeAt + 1);
					return true;
				}
			}
		}
		return false;
	}

	void Startup::Tick()
	{
		if (!toComplete)
		{
			return;
		}
		for (auto i = 0U; i < getStartupInfos.size(); ++i)
		{
			// * TODO-REDO_UI: Reset backend user if the main Startup request fails.
			auto setMotd = i == 0;
			auto setError = i == 0;
			auto setStatus = i == 0;
			auto &req = getStartupInfos[i];
			if (req && req->complete)
			{
				toComplete -= 1;
				// * TODO-REDO_UI
				if (req->status)
				{
					if (setMotd)
					{
						motdText = req->motd;
						String text, uri;
						if (GetMotdUri(text, uri, motdText))
						{
							motdText = text;
							motdUri = uri;
						}
					}
				}
				else
				{
					if (setError)
					{
						error = req->error;
					}
				}
				if (setStatus)
				{
					status = req->status;
				}
				req.reset();
			}
		}
	}
}
