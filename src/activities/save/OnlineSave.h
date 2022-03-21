#pragma once
#include "Config.h"

#include "Save.h"
#include "backend/SaveInfo.h"

#include <memory>

namespace backend
{
	class PutSave;
}

namespace gui
{
	class CheckBox;
}

namespace activities::save
{
	class OnlineSave : public Save
	{
		gui::TextBox *descriptionBox = nullptr;
		gui::CheckBox *publishCheck = nullptr;
		gui::CheckBox *pausedCheck = nullptr;

		std::shared_ptr<backend::PutSave> putSave;

		void UpdateTasks();

	public:
		OnlineSave(backend::SaveInfo info, std::shared_ptr<GameSave> saveData);

		void Tick() final override;
	};
}
