#pragma once
#include "Config.h"

#include "gui/PopupWindow.h"

#include <vector>
#include <memory>

namespace gui
{
	class Static;
	class ProgressBar;
}

namespace common
{
	struct Task;
}

namespace activities::taskbatch
{
	struct TaskInfo
	{
		std::shared_ptr<common::Task> task;
		String description;
		String errorPrefix;
	};

	class TaskBatch : public gui::PopupWindow
	{
	public:
		using FinishCallback = std::function<void ()>;

	private:
		float overallWeight = 0.f;
		float completedWeight = 0.f;
		float overallProgress = 0.f;
		bool overallProgressIndeterminate;
		int taskToStart = 0;
		std::vector<TaskInfo> tasks;
		TaskInfo *currentTask = nullptr;

		FinishCallback finish;

		gui::Static *descriptionStatic = nullptr;
		gui::ProgressBar *progress = nullptr;

	public:
		// * TODO: Dependency graph instead of newBailOnFailure?
		TaskBatch(String batchDescription, std::vector<TaskInfo> newTasks);

		void Tick() final override;

		void Finish(FinishCallback newFinish)
		{
			finish = newFinish;
		}
		FinishCallback Finish() const
		{
			return finish;
		}
	};
}
