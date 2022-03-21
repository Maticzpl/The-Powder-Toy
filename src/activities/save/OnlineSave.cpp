#include "OnlineSave.h"

#include "Metrics.h"
#include "gui/Appearance.h"
#include "gui/Button.h"
#include "gui/CheckBox.h"
#include "gui/Static.h"
#include "gui/TextBox.h"
#include "gui/Texture.h"
#include "language/Language.h"
#include "simulation/GameSave.h"
#include "backend/PutSave.h"
#include "saveonline.bmp.h"

namespace activities::save
{
	constexpr auto windowSize = gui::Point{ 361, 159 };

	OnlineSave::OnlineSave(backend::SaveInfo info, std::shared_ptr<GameSave> saveData) : Save(info.name, saveData, windowSize)
	{
		title->Text("DEFAULT_LS_ONLINESAVE_TITLE"_Ls());
		background->ImageData(gui::Image::FromBMP(reinterpret_cast<const char *>(saveOnline), saveOnlineSize));
		backgroundRect.size = background->ImageData().Size();

		nameBox->Focus();

		descriptionBox = EmplaceChild<gui::TextBox>().get();
		descriptionBox->PlaceHolder(String::Build("[", "DEFAULT_LS_ONLINESAVE_DESCRIPTION"_Ls(), "]"));
		descriptionBox->Position({ 119, 52 });
		descriptionBox->Size({ 234, 82 });
		descriptionBox->Multiline(true);

		auto *infoButton = EmplaceChild<gui::Button>().get();
		infoButton->Text("DEFAULT_LS_ONLINESAVE_INFO"_Ls());
		infoButton->Position({ 59, windowSize.y - 17 });
		infoButton->Size({ 122, 17 });
		infoButton->TextColor(gui::Appearance::informationTitle);

		auto *rulesButton = EmplaceChild<gui::Button>().get();
		rulesButton->Text("DEFAULT_LS_ONLINESAVE_RULES"_Ls());
		rulesButton->Position({ 180, windowSize.y - 17 });
		rulesButton->Size({ 122, 17 });
		rulesButton->TextColor(gui::Appearance::informationTitle);

		publishCheck = EmplaceChild<gui::CheckBox>().get();
		publishCheck->Text("DEFAULT_LS_ONLINESAVE_PUBLISH"_Ls());
		publishCheck->Position({ 8, 102 });
		publishCheck->Size({ 105, 14 });

		pausedCheck = EmplaceChild<gui::CheckBox>().get();
		pausedCheck->Text("DEFAULT_LS_ONLINESAVE_PAUSED"_Ls());
		pausedCheck->Position({ 8, 120 });
		pausedCheck->Size({ 105, 14 });

		cancelButton->Position({ 0, windowSize.y - 17 });
		cancelButton->Size({ 60, 17 });

		saveButton->Position({ 301, windowSize.y - 17 });
		saveButton->Size({ 60, 17 });
		saveButton->Click([this, saveData] {
			assert(!saveData->Collapsed());
			saveData->paused = pausedCheck->Value();
			saveData->Collapse();
			saveButton->Enabled(false);
			cancelButton->Enabled(false);
			putSave = std::make_shared<backend::PutSave>(nameBox->Text(), descriptionBox->Text(), publishCheck->Value(), saveData->Serialise());
			putSave->Start(putSave);
		});

		Size(windowSize);
		Position((parentSize - windowSize) / 2);
	}

	void OnlineSave::UpdateTasks()
	{
		if (putSave && putSave->complete)
		{
			if (putSave->status)
			{
				Quit();
			}
			// * TODO-REDO_UI: Do something with this.
			putSave.reset();
		}
	}

	void OnlineSave::Tick()
	{
		Save::Tick();
		UpdateTasks();
	}
}
