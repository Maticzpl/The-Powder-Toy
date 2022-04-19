#include "RequestManager.h"

#include "Request.h"

namespace network
{
	struct RequestInfo
	{
	};

	RequestManager::RequestManager()
	{
	}

	RequestManager::~RequestManager()
	{
	}

	void RequestManager::Tick()
	{
	}

	void RequestManager::Add(Request *request)
	{
		request->responseCode = 604;
		request->status = Request::statusDone;
	}

	void RequestManager::Remove(Request *request)
	{
	}
}
