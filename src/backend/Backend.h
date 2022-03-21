#pragma once
#include "Config.h"

#include "common/String.h"
#include "common/ExplicitSingleton.h"
#include "UserInfo.h"
#include "BackendTask.h"

#include <memory>

namespace network
{
	struct Request;
}

namespace backend
{
	class Backend : public common::ExplicitSingleton<Backend>
	{
		UserInfo user{};
		std::vector<std::weak_ptr<BackendTask>> tasks;

	public:
		Backend();

		void User(UserInfo newUser);
		const UserInfo &User() const
		{
			return user;
		}

		void Tick();

		void Dispatch(std::shared_ptr<BackendTask> task);
		void AddAuthHeaders(network::Request &request) const;
	};
}
