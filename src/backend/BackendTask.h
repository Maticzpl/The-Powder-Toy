#pragma once
#include "Config.h"

#include "common/Task.h"
#include "json/json.h"

#include <memory>

namespace network
{
	struct Request;
}

namespace backend
{
	class Backend;

	class BackendTask : public common::Task
	{
		virtual bool Process() = 0;

	protected:
		std::shared_ptr<network::Request> request;

		enum ResponseCheckMode
		{
			responseCheckJson,  // * Response body is JSON, ["Status"] == 1 if everything went alright.
			responseCheckOk,    // * Response body is plain text, says OK if everything went right.
			responseCheckBasic, // * Response body is the requested data, status code and other nonsense determines whether the request succeeded.
			responseCheckMax,   // * Must be the last enum constant.
		};
		bool PreprocessResponse(ResponseCheckMode responseCheckMode);

		Json::Value root;

	public:
		BackendTask();
		virtual ~BackendTask() = default;

		void Start(std::shared_ptr<common::Task> self) final override;

		friend class Backend;
	};
}
