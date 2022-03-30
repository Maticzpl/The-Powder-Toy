#include "TaskBatch.h"

#include "common/Task.h"
#include "gui/Appearance.h"
#include "gui/Static.h"
#include "gui/ProgressBar.h"
#include "gui/Separator.h"
#include "gui/SDLWindow.h"

#include <iostream>

namespace activities::taskbatch
{
	constexpr auto parentSize = gui::Point{ WINDOWW, WINDOWH };
	constexpr auto windowSize = gui::Point{ 200, 73 };

	TaskBatch::TaskBatch(String batchDescription, std::vector<TaskInfo> newTasks) : tasks(newTasks)
	{
		Size(windowSize);
		Quittable(false);
		Position((parentSize - windowSize) / 2);

		{
			auto title = EmplaceChild<gui::Static>();
			title->Position({ 6, 5 });
			title->Size({ windowSize.x - 20, 14 });
			title->Text(batchDescription);
			title->TextColor(gui::Appearance::informationTitle);
			auto sep = EmplaceChild<gui::Separator>();
			sep->Position({ 1, 22 });
			sep->Size({ windowSize.x - 2, 1 });
		}

		descriptionStatic = EmplaceChild<gui::Static>().get();
		descriptionStatic->Size({ windowSize.x - 16, 15 });
		descriptionStatic->Position({ 8, 28 });

		progress = EmplaceChild<gui::ProgressBar>().get();
		progress->Size({ windowSize.x - 16, 17 });
		progress->Position({ 8, 47 });

		for (auto &task : tasks)
		{
			overallWeight += task.task->relativeWeight;
		}
	}

	void TaskBatch::Tick()
	{
		PopupWindow::Tick();
		if (!currentTask)
		{
			currentTask = &tasks[taskToStart];
			currentTask->task->Start(tasks[taskToStart].task);
			descriptionStatic->Text(currentTask->description);
		}
		if (currentTask->task->complete)
		{
			if (!currentTask->task->status)
			{
				std::cerr << currentTask->errorPrefix.ToUtf8() << currentTask->task->status.error.ToUtf8() << std::endl;
			}
			completedWeight += currentTask->task->relativeWeight;
			currentTask = nullptr;
			taskToStart += 1;
		}
		overallProgress = completedWeight;
		overallProgressIndeterminate = false;
		if (currentTask)
		{
			overallProgress += currentTask->task->progress * currentTask->task->relativeWeight;
			overallProgressIndeterminate = currentTask->task->progressIndeterminate;
		}
		overallProgress /= overallWeight;
		if (progress->Progress() != overallProgress)
		{
			progress->Progress(overallProgress);
		}
		if (progress->Indeterminate() != overallProgressIndeterminate)
		{
			progress->Indeterminate(overallProgressIndeterminate);
		}
		if (taskToStart == int(tasks.size()))
		{
			gui::SDLWindow::Ref().PopBack(this);
			// * TODO-REDO_UI: Display errors that were accumulated.
			if (finish)
			{
				finish();
			}
		}
	}
}
