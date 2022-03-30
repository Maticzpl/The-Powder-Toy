#pragma once
#include "Config.h"

#include "ExplicitSingleton.h"
#include "Task.h"

#include <memory>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <functional>

namespace common
{
	struct WorkerTask : public common::Task
	{
		virtual ~WorkerTask() = default;

		virtual Status Process() = 0;

		void Start(std::shared_ptr<common::Task> self) final override;
	};

	class Worker : public ExplicitSingleton<Worker>
	{
		std::mutex workerMx;
		std::condition_variable workerCv;
		std::thread workerTh;
		std::queue<std::weak_ptr<WorkerTask>> workerQu;
		bool running = true;

	public:
		Worker();
		~Worker();

		void Dispatch(std::shared_ptr<WorkerTask> task);
	};
}
