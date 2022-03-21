#include "Request.h"

#include "RequestManager.h"

namespace network
{
	Request::~Request()
	{
		Cancel();
	}

	void Request::Dispatch()
	{
		if (status == statusIdle)
		{
			status = statusRunning;
			RequestManager::Ref().Add(this);
		}
	}

	void Request::Cancel()
	{
		if (status == statusRunning)
		{
			RequestManager::Ref().Remove(this);
			status = statusCancelled;
		}
	}
}
