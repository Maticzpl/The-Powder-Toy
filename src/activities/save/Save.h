#pragma once
#include "Config.h"

#include "gui/PopupWindow.h"
#include "common/String.h"

#include <memory>

class GameSave;

namespace graphics
{
	class ThumbnailRendererTask;
}

namespace gui
{
	class ImageButton;
	class Button;
	class Static;
	class TextBox;
	class Spinner;
	class Texture;
}

namespace activities::save
{
	class Save : public gui::PopupWindow
	{
	protected:
		gui::Point targetSize = { 0, 0 };
		gui::Texture *background = nullptr;
		gui::Rect backgroundRect = { { 0, 0 }, { 0, 0 } };

		gui::Button *saveButton = nullptr;
		gui::Button *cancelButton = nullptr;
		gui::TextBox *nameBox = nullptr;
		gui::Static *title = nullptr;
		gui::ImageButton *thumbnail = nullptr;
		gui::Spinner *thumbnailSpinner = nullptr;

		std::shared_ptr<const GameSave> saveData;
		std::shared_ptr<graphics::ThumbnailRendererTask> saveRender;
		void UpdateTasks();

		Save(String name, std::shared_ptr<const GameSave> saveData, gui::Point windowSize);

	public:
		~Save();

		virtual void Tick() override;
		void Draw() const final override;
	};
}
