#pragma once
#include "Config.h"

#include "common/String.h"
#include "common/ExplicitSingleton.h"

#include <memory>
#include <array>

namespace backend
{
	class GetStartupInfo;

	class Startup : public common::ExplicitSingleton<Startup>
	{
		int toComplete = 0;
		bool status;
		String error;
		String motdText;
		String motdUri;

#ifdef UPDATESERVER
		std::array<std::shared_ptr<backend::GetStartupInfo>, 2> getStartupInfos;
#else
		std::array<std::shared_ptr<backend::GetStartupInfo>, 1> getStartupInfos;
#endif

	public:
		Startup();

		bool Complete() const
		{
			return toComplete == 0;
		}

		bool Status() const
		{
			return status;
		}

		const String &MotdText() const
		{
			return motdText;
		}

		const String &Error() const
		{
			return error;
		}

		const String &MotdUri() const
		{
			return motdUri;
		}

		void Tick();
	};
}
