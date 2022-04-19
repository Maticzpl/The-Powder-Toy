#pragma once

#include "Config.h"
#include "common/String.h"

#include <vector>
#include <map>

namespace network
{
	struct Request
	{
		bool post = false;
		ByteString uri;
		ByteString responseBody;
		int responseCode;
		int total = 0;
		int done = 0;
		enum Status
		{
			statusIdle,
			statusRunning,
			statusDone,
			statusCancelled,
			statusMax,
		};
		Status status = statusIdle;
		std::map<ByteString, ByteString> headers;
		std::map<ByteString, ByteString> postFields;

		~Request();
		void Dispatch();
		void Cancel();
	};
}
