#pragma once
#include "Config.h"

#include "common/ExplicitSingleton.h"
#include "common/String.h"

#include <vector>
#include <memory>

namespace network
{
	struct Request;
	struct RequestInfo;

	class RequestManager : public common::ExplicitSingleton<RequestManager>
	{
		bool disableNetwork = false;
		ByteString proxy;
		int timeout = 15;
		void *opaque;
		std::vector<std::unique_ptr<RequestInfo>> requests;

	public:
		RequestManager();
		~RequestManager();

		void Tick();

		void Add(Request *request);
		void Remove(Request *request);

		void DisableNetwork(bool newDisableNetwork)
		{
			disableNetwork = newDisableNetwork;
		}
		bool DisableNetwork() const
		{
			return disableNetwork;
		}

		void Proxy(ByteString newProxy)
		{
			proxy = newProxy;
		}
		ByteString Proxy() const
		{
			return proxy;
		}

		void Timeout(int newTimeout)
		{
			timeout = newTimeout;
		}
		int Timeout() const
		{
			return timeout;
		}
	};
}
