#pragma once
#include "Config.h"

#include "Save.h"

#include <memory>

namespace activities::save
{
	class WriteSaveTask;

	class LocalSave : public Save
	{
		std::shared_ptr<WriteSaveTask> writeSave;

		void UpdateTasks();

	public:
		LocalSave(String name, std::shared_ptr<GameSave> saveData);

		void Tick() final override;
	};
}
