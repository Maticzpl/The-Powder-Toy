#pragma once
#include "Config.h"

#include "common/ExplicitSingleton.h"

#ifndef NOHTTP
# include <vector>
# include <memory>
#else
# include "Request.h"
#endif

namespace network
{
	struct Request;
	struct RequestInfo;

	class RequestManager : public common::ExplicitSingleton<RequestManager>
	{
#ifndef NOHTTP
		void *opaque;
		std::vector<std::unique_ptr<RequestInfo>> requests;

	public:
		RequestManager();
		~RequestManager();

		void Tick();

		void Add(Request *request);
		void Remove(Request *request);
#else
	public:
		void Add(Request *request)
		{
			request->responseCode = 604;
			request->status = Request::statusDone;
		}

		void Remove(Request *request)
		{
		}
#endif
	};
}
