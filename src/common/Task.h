#pragma once

#include "Config.h"
#include "common/String.h"

#include <memory>
#include <atomic>

namespace common
{
	// * This could have been an std::promise, but that wouldn't be able to
	//   report progress. Then again, the only type of Task that can report
	//   progress is BackendTask, which could probably just expose its own
	//   interface. I'm not sure, I may yet migrate to std::promise.
	struct Task
	{
		struct Status
		{
			bool ok = false;
			String shortError;
			String error;

			operator bool() const
			{
				return ok;
			}
		};

		std::atomic<bool> complete = false;
		Status status;

		float relativeWeight = 1.f;
		std::atomic<float> progress = 0.f;
		std::atomic<bool> progressIndeterminate = false;

		virtual void Start(std::shared_ptr<Task> self) = 0;

		virtual ~Task() = default;
	};
}
