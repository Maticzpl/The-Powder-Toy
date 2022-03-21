#include "Worker.h"

#include <iostream>

namespace common
{
	Worker::Worker()
	{
		workerTh = std::thread([this]() {
			while (true)
			{
				std::weak_ptr<WorkerTask> wptr;
				{
					std::unique_lock<std::mutex> ul(workerMx);
					while (true)
					{
						if (!workerQu.empty())
						{
							wptr = workerQu.front();
							workerQu.pop();
							break;
						}
						if (!running)
						{
							return;
						}
						workerCv.wait(ul);
					}
				}
				auto ptr = wptr.lock();
				if (ptr)
				{
					if (ptr->process)
					{
						ptr->process(*ptr);
					}
					ptr->complete = true;
				}
				else
				{
					std::cerr << "Worker: task abandoned" << std::endl;
				}
			}
		});
	}

	Worker::~Worker()
	{
		{
			std::unique_lock<std::mutex> ul(workerMx);
			running = false;
		}
		workerCv.notify_all();
		workerTh.join();
	}

	void Worker::Dispatch(std::shared_ptr<WorkerTask> task)
	{
		{
			std::unique_lock<std::mutex> ul(workerMx);
			workerQu.push(std::weak_ptr(task));
		}
		workerCv.notify_one();
	}
}
