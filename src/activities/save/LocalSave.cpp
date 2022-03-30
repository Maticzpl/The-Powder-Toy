#include "LocalSave.h"

#include "Metrics.h"
#include "activities/game/Game.h"
#include "gui/Button.h"
#include "gui/Icons.h"
#include "gui/Static.h"
#include "gui/SDLWindow.h"
#include "gui/TextBox.h"
#include "gui/Texture.h"
#include "language/Language.h"
#include "simulation/GameSave.h"
#include "common/Worker.h"
#include "common/Path.h"
#include "common/Platform.h"
#include "savelocal.bmp.h"

#include <iostream>

namespace activities::save
{
	constexpr auto windowSize = gui::Point{ 261, 122 };

	class WriteSaveTask : public common::WorkerTask
	{
		const Path path;
		std::shared_ptr<const GameSave> save;

		common::Task::Status Process() final override
		{
			auto data = save->Serialise();
			if (!data.size())
			{
				// * TODO-REDO_UI: Do something with this.
				return { false, "failed to serialize save", "" };
			}
			if (!Platform::WriteFile(data, String(path).ToUtf8()))
			{
				// * TODO-REDO_UI: Do something with this.
				return { false, "failed to write save", "" };
			}
			return { true };
		}

	public:
		WriteSaveTask(const Path newPath, std::shared_ptr<const GameSave> newSave) : path(newPath), save(newSave)
		{
			progressIndeterminate = true;
		}
	};

	LocalSave::LocalSave(String name, std::shared_ptr<GameSave> saveData) : Save(name, saveData, windowSize)
	{
		title->Text("DEFAULT_LS_LOCALSAVE_TITLE"_Ls());
		background->ImageData(gui::Image::FromBMP(reinterpret_cast<const char *>(saveLocal), saveLocalSize));
		backgroundRect.size = background->ImageData().Size();

		nameBox->Focus();

		cancelButton->Position({ 0, windowSize.y - 17 });
		cancelButton->Size({ 131, 17 });

		// * TODO-REDO_UI: Set correct path, also add tooltip with full path.
		auto *foldersStatic = EmplaceChild<gui::Static>().get();
		{
			foldersStatic->Text("DEFAULT_LS_LOCALSAVE_DESTINATION"_Ls(Path::FromString(LOCAL_SAVE_DIR).Append(activities::game::Game::Ref().SavesPath()).Components().back()));
		}
		foldersStatic->Position({ 119, 61 });
		foldersStatic->Size({ 134, 17 });

		auto *foldersButton = EmplaceChild<gui::Button>().get();
		foldersButton->Text(String::Build(gui::ColorString({ 0xC0, 0xA0, 0x40 }), gui::IconString(gui::Icons::folderBody   , { 1, -2 }), gui::StepBackString(),
		                                  gui::ColorString({ 0xFF, 0xFF, 0xFF }), gui::IconString(gui::Icons::folderOutline, { 1, -2 }), " ", "DEFAULT_LS_LOCALSAVE_FOLDER"_Ls()));
		foldersButton->Position({ 119, 80 });
		foldersButton->Size({ 134, 17 });

		saveButton->Position({ 130, windowSize.y - 17 });
		saveButton->Size({ 131, 17 });
		saveButton->Click([this, saveData] {
			saveData->Collapse();
			saveButton->Enabled(false);
			cancelButton->Enabled(false);
			Path path = Path::FromString(LOCAL_SAVE_DIR).Append(activities::game::Game::Ref().SavesPath());
			try
			{
				path = path.Append(nameBox->Text() + ".cps");
			}
			catch (const Path::FormatError &)
			{
				return;
			}
			writeSave = std::make_shared<WriteSaveTask>(path, saveData);
			writeSave->Start(writeSave);
		});
	}

	void LocalSave::UpdateTasks()
	{
		if (writeSave && writeSave->complete)
		{
			if (writeSave->status)
			{
				Quit();
			}
			// * TODO-REDO_UI: Do something with this.
			writeSave.reset();
		}
	}

	void LocalSave::Tick()
	{
		Save::Tick();
		UpdateTasks();
	}
}
